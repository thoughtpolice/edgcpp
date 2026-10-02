#include "qbe_codegen.h"
#include "il.h"
#include "mem_manage.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static uint64_t rounded(uint64_t n, unsigned a) { return (n + a - 1) & ~((uint64_t)a - 1); }
static a_boolean record(a_type_ptr t) {
  t = qbe_underlying(t);
  return t && (t->kind == tk_class || t->kind == tk_struct || t->kind == tk_union);
}
static a_boolean memory_value(a_type_ptr t) { return qbe_aggregate(t) || qbe_size(t) > 8; }
static unsigned argument_alignment(a_type_ptr t) {
  a_field_ptr f;
  unsigned result = 1;
  t = qbe_underlying(t);
  if (!record(t)) {
    uint64_t n;
    if (t->kind == tk_array) return argument_alignment(t->variant.array.element_type);
    if (t->kind == tk_integer || t->kind == tk_float || t->kind == tk_pointer || t->kind == tk_vector) {
      n = qbe_size(t); return (unsigned)(n < 16 ? n : 16);
    }
    return qbe_alignment(t);
  }
  for (f = t->variant.class_struct_union.field_list; f; f = f->next) {
    unsigned member = qbe_alignment(f->type);
    if (f->alignment) member = f->alignment;
    else {
      unsigned packing;
#if GNU_EXTENSIONS_ALLOWED
      if (f->is_packed) member = 1;
#endif
      packing = t->variant.class_struct_union.max_member_alignment;
      if (packing && member > packing) member = packing;
    }
    if (member > result) result = member;
  }
  return result;
}
static unsigned hidden_result_index(a_type_ptr t) {
  a_routine_type_supplement_ptr info = qbe_underlying(t)->variant.routine.extra_info;
  return info->return_value_parameter_follows_this && info->this_class ? 1 : 0;
}
static a_boolean contains_bridged_float(a_type_ptr t) {
  a_field_ptr f;
  t = qbe_underlying(t);
  if (!t) return FALSE;
  if (t->kind == tk_float) return qbe_size(t) == 16 || qbe_narrow_float(t);
  if (t->kind == tk_array) return contains_bridged_float(t->variant.array.element_type);
  if (record(t)) for (f = t->variant.class_struct_union.field_list; f; f = f->next)
    if (contains_bridged_float(f->type)) return TRUE;
  return FALSE;
}
static a_boolean needs_bridge(QbeBackend *b, a_type_ptr t) {
  QbeNativeClass c;
  size_t i;
  t = qbe_underlying(t);
  if (contains_bridged_float(t) || t->kind == tk_vector ||
      (t->kind == tk_integer && qbe_size(t) > 8) ||
      (qbe_aggregate(t) && qbe_alignment(t) > 8)) return TRUE;
  c = qbe_native_class(b, t);
  if (qbe_aggregate(t) && c.bytes == 0) return TRUE;
  if (c.floating_count && qbe_aggregate(t)) {
    for (i = 0; i < c.floating_count; ++i)
      if (qbe_underlying(c.floating[i].type)->kind == tk_vector) return TRUE;
    return c.floating_count * qbe_size(c.floating[0].type) != qbe_size(t);
  }
  return FALSE;
}
static a_boolean wide_signature(QbeBackend *b, a_type_ptr t, size_t argc, const QbeValue *args) {
  a_param_type_ptr p;
  size_t i;
  t = qbe_underlying(t);
  if (needs_bridge(b, t->variant.routine.return_type)) return TRUE;
  for (p = t->variant.routine.extra_info->param_type_list; p; p = p->next)
    if (needs_bridge(b, p->type)) return TRUE;
  for (i = 0; i < argc; ++i) if (needs_bridge(b, args[i].type)) return TRUE;
  return FALSE;
}
static a_boolean same_floating_format(a_type_ptr a, a_type_ptr b) {
  a = qbe_underlying(a); b = qbe_underlying(b);
  if (qbe_size(a) != qbe_size(b) || a->kind != b->kind) return FALSE;
  return a->kind != tk_float ||
    (a->variant.float_kind == fk_std_bfloat16) == (b->variant.float_kind == fk_std_bfloat16);
}
/* Actual target offsets determine HFA layout, including union alternatives. */
static a_boolean floating_leaves(a_type_ptr t, uint64_t off, QbeNativeClass *leaves) {
  a_field_ptr f;
  t = qbe_underlying(t);
  if (t->kind == tk_float || (t->kind == tk_vector && qbe_size(t) <= 16)) {
    size_t i = leaves->floating_count;
    if (i == 4) return FALSE;
    leaves->floating[i].type = t; leaves->floating[i].offset = off;
    leaves->floating_count++;
    return TRUE;
  }
  if (t->kind == tk_array) {
    a_type_ptr e = t->variant.array.element_type;
    uint64_t i, n = t->variant.array.variant.number_of_elements;
    if (n > 4) return FALSE;
    for (i = 0; i < n; ++i) if (!floating_leaves(e, off + i * qbe_size(e), leaves)) return FALSE;
    return TRUE;
  }
  if (!record(t)) return FALSE;
  if (t->kind == tk_union) {
    QbeNativeClass largest = {0};
    uint64_t element_size = 0;
    a_type_ptr element_type = NULL;
    for (f = t->variant.class_struct_union.field_list; f; f = f->next) {
      QbeNativeClass alternative = {0};
      uint64_t s;
      size_t i;
      if (f->is_bit_field && f->bit_size == 0) continue;
      if (f->is_bit_field || !floating_leaves(f->type, off + f->offset, &alternative)) return FALSE;
      if (!alternative.floating_count) continue;
      s = qbe_size(alternative.floating[0].type);
      for (i = 0; i < alternative.floating_count; ++i)
        if (!same_floating_format(alternative.floating[i].type, alternative.floating[0].type)) return FALSE;
      if (element_size && (element_size != s || !same_floating_format(element_type, alternative.floating[0].type))) return FALSE;
      element_size = s; element_type = alternative.floating[0].type;
      if (alternative.floating_count > largest.floating_count) largest = alternative;
    }
    if (leaves->floating_count + largest.floating_count > 4) return FALSE;
    memcpy(leaves->floating + leaves->floating_count, largest.floating,
           largest.floating_count * sizeof(largest.floating[0]));
    leaves->floating_count += largest.floating_count;
    return TRUE;
  }
  for (f = t->variant.class_struct_union.field_list; f; f = f->next) {
    if (f->is_bit_field && f->bit_size == 0) continue;
    if (f->is_bit_field || !floating_leaves(f->type, off + f->offset, leaves)) return FALSE;
  }
  return TRUE;
}
QbeNativeClass qbe_native_class(QbeBackend *b, a_type_ptr type) {
  QbeNativeClass c = {0}, leaves = {0};
  size_t i;
  unsigned align;
  (void)b;
  type = qbe_underlying(type);
  c.bytes = qbe_size(type);
  align = argument_alignment(type);
  c.align = align < 8 ? 8 : align > 16 ? 16 : align;
  if (floating_leaves(type, 0, &leaves) && leaves.floating_count) {
    a_boolean homogeneous = TRUE;
    for (i = 0; i < leaves.floating_count; ++i)
      if (!same_floating_format(leaves.floating[i].type, leaves.floating[0].type)) homogeneous = FALSE;
    if (homogeneous) {
      c.floating_count = leaves.floating_count;
      memcpy(c.floating, leaves.floating, sizeof(c.floating));
    }
  }
  if (!c.floating_count && qbe_aggregate(type) && c.bytes > 16) {
    c.indirect = TRUE; c.bytes = 8; c.align = 8;
  }
  return c;
}
typedef struct { uint64_t packet, value, bytes; } Piece;
typedef struct { a_boolean indirect; size_t count; Piece pieces[8]; } Placement;
typedef struct { unsigned gp, fp; uint64_t stack; } Registers;
static Placement place(QbeBackend *b, a_type_ptr type, Registers *r) {
  QbeNativeClass c = qbe_native_class(b, type);
  Placement p = {0};
  unsigned i, n;
  p.indirect = c.indirect;
  if (c.floating_count) {
    n = (unsigned)c.floating_count;
    if (r->fp + n <= 8) {
      for (i = 0; i < n; ++i) p.pieces[p.count++] = (Piece){80 + 16 * r->fp++, c.floating[i].offset, qbe_size(c.floating[i].type)};
      return p;
    }
    r->fp = 8;
  } else {
    if (c.align == 16) r->gp = (r->gp + 1) & ~1u;
    n = (unsigned)(rounded(c.bytes, 8) / 8);
    if (r->gp + n <= 8) {
      for (i = 0; i < n; ++i) {
        uint64_t bytes = c.bytes - 8 * (uint64_t)i;
        p.pieces[p.count++] = (Piece){16 + 8 * r->gp++, 8 * (uint64_t)i, bytes < 8 ? bytes : 8};
      }
      return p;
    }
    r->gp = 8;
  }
  r->stack = rounded(r->stack, c.align);
  p.pieces[p.count++] = (Piece){208 + r->stack, 0, c.bytes};
  r->stack += rounded(c.bytes, 8);
  return p;
}
static Placement hidden_placement(void) {
  Placement p = {0}; p.count = 1; p.pieces[0] = (Piece){8, 0, 8}; return p;
}
static a_scope_ptr routine_scope(a_routine_ptr routine) {
  a_memory_region_number region = mem_region_for_routine(routine);
  if (region == NULL_region_number || !mem_region_table[region]) return NULL;
  return scope_for_routine(routine);
}

const char *qbe_var_address(QbeBackend *b, a_variable_ptr variable) {
  if (variable->is_vla)
    return qbe_emit(b, "l", qbe_concat(b, "loadl ", qbe_vla_cell(b, variable), NULL));
  const char *found = qbe_map_get(&b->variables_, variable);
  a_boolean tls;
  const char *slot;
  uint64_t bytes;
  if (found) return found;
  tls = is_effective_thread_local(variable);
#if GNU_EXTENSIONS_ALLOWED && DECL_MODIFIERS_IN_USE
  tls = tls || (variable->decl_modifiers & DM_THREAD) != 0;
#endif
  if (tls) {
    qbe_emit_variable(b, variable);
    return qbe_emit(b, "l", qbe_concat(b, "copy extern thread $", qbe_variable_symbol(b, variable), NULL));
  }
  if (variable->storage_class != sc_auto && variable->storage_class != sc_register && !variable->is_parameter) {
    qbe_emit_variable(b, variable);
    return qbe_concat(b, "$", qbe_variable_symbol(b, variable), NULL);
  }
  bytes = qbe_size(variable->type);
  slot = qbe_allocate(b, bytes ? bytes : 1, qbe_alignment(variable->type));
  qbe_map_put(b, &b->variables_, variable, slot);
  return slot;
}

QbeValue qbe_emit_call(QbeBackend *b, a_routine_ptr direct_routine,
                      a_type_ptr routine_type, const char *target,
                      size_t argc, const QbeValue *arguments, a_boolean may_throw) {
  a_routine_type_supplement_ptr rtsp;
  a_type_ptr result_type;
  a_routine_ptr inline_routine = NULL;
  a_param_type_ptr parameter;
  size_t count = 0, i;
  QbeValue *actual;
  a_boolean wide;
  unsigned hidden;
  routine_type = qbe_underlying(routine_type);
  if (routine_type->kind == tk_pointer) routine_type = qbe_underlying(routine_type->variant.pointer.type);
  if (routine_type->kind != tk_routine) qbe_fail("call target does not have a routine type");
  rtsp = routine_type->variant.routine.extra_info;
  result_type = routine_type->variant.routine.return_type;
  if (b->inlineForwardRoutine_ &&
      !strcmp(target, qbe_concat(b, "$", qbe_routine_symbol(b, b->inlineForwardRoutine_), NULL)))
    inline_routine = b->inlineForwardRoutine_;
  else if (direct_routine && direct_routine->suppress_inline_body) {
    if (rtsp->has_ellipsis) qbe_fail("cannot expand a separate variadic inline-only routine");
    inline_routine = direct_routine;
  }
  if (inline_routine) {
    a_scope_ptr master_scope = routine_scope(inline_routine);
    a_routine_ptr saved_routine = b->routine_, saved_forward = b->inlineForwardRoutine_;
    a_scope_ptr saved_scope = b->scope_;
    a_variable_ptr saved_result = b->resultParameter_, p;
    const char *saved_label = b->inlineReturnLabel_, *saved_slot = b->inlineReturnSlot_;
    a_type_ptr saved_type = b->inlineReturnType_;
    QbeValue result;
    if (b->routine_ == inline_routine)
      qbe_fail(qbe_concat(b, "recursive inline-only routine ", qbe_map_get(&b->symbols_, qbe_routine_symbol(b, inline_routine)), NULL));
    if (!master_scope)
      qbe_fail(qbe_concat(b, "inline-only routine has no resident body: ", qbe_map_get(&b->symbols_, qbe_routine_symbol(b, inline_routine)), NULL));
    b->inlineForwardRoutine_ = NULL;
    b->inlineReturnLabel_ = qbe_new_label(b, "forward_return");
    b->inlineReturnType_ = result_type;
    b->inlineReturnSlot_ = *qbe_base_type(b, result_type) ? qbe_allocate(b, qbe_size(result_type), qbe_alignment(result_type)) : "";
    b->routine_ = inline_routine; b->scope_ = master_scope; b->resultParameter_ = NULL;
    hidden = hidden_result_index(routine_type);
    for (p = master_scope->variant.routine.parameters, i = 0; p; p = p->next, ++i) {
      if (i >= argc) qbe_fail("missing variadic master parameter");
      qbe_store(b, p->type, qbe_cast(b, arguments[i], p->type), qbe_var_address(b, p));
      if (rtsp->value_returned_as_parameter && i == hidden) b->resultParameter_ = p;
    }
    qbe_begin_inline_eh(b);
    qbe_statement(b, master_scope->assoc_block);
    qbe_finish_inline_eh(b);
    if (!b->terminated_) qbe_jump(b, b->inlineReturnLabel_);
    qbe_label(b, b->inlineReturnLabel_);
    result = *b->inlineReturnSlot_ ? qbe_load(b, result_type, b->inlineReturnSlot_) : qbe_value(result_type, "");
    b->routine_ = saved_routine; b->scope_ = saved_scope; b->resultParameter_ = saved_result;
    b->inlineForwardRoutine_ = saved_forward; b->inlineReturnLabel_ = saved_label;
    b->inlineReturnSlot_ = saved_slot; b->inlineReturnType_ = saved_type;
    return result;
  }
  for (parameter = rtsp->param_type_list; parameter; parameter = parameter->next) ++count;
  if (argc < count) {
    const char *callee = *target == '$' ? qbe_map_get(&b->symbols_, target + 1) : target;
    qbe_fail(qbe_format(b, "call to %s in %s supplies %zu arguments for %zu lowered parameters",
      callee, b->routine_ && b->routine_->source_corresp.name ? b->routine_->source_corresp.name : "<runtime>", argc, count));
  }
  actual = qbe_alloc(b, argc * sizeof(*actual));
  parameter = rtsp->param_type_list;
  for (i = 0; i < argc; ++i) {
    actual[i] = parameter ? qbe_cast(b, arguments[i], parameter->type) : arguments[i];
    if (parameter) parameter = parameter->next;
  }
  wide = wide_signature(b, routine_type, argc, actual);
  hidden = hidden_result_index(routine_type);
  if (wide) {
    Registers regs = {0};
    Placement *places = qbe_alloc(b, argc * sizeof(*places));
    const char *packet, *image, *result_storage = "";
    QbeNativeClass result_class;
    unsigned site;
    for (i = 0; i < argc; ++i)
      places[i] = rtsp->value_returned_as_parameter && i == hidden ? hidden_placement() : place(b, actual[i].type, &regs);
    packet = qbe_allocate(b, 208 + rounded(regs.stack, 16), 16);
    image = qbe_allocate(b, 80, 16);
    qbe_zero(b, packet, 208 + rounded(regs.stack, 16));
    qbe_store(b, qbe_integer_type(b, 64, FALSE),
      qbe_value(qbe_integer_type(b, 64, FALSE), qbe_format(b, "%llu", (unsigned long long)regs.stack)), packet);
    result_class = qbe_native_class(b, result_type);
    if (result_class.indirect) {
      result_storage = qbe_allocate(b, qbe_size(result_type), qbe_alignment(result_type));
      qbe_store(b, qbe_pointer_type(b), qbe_value(qbe_pointer_type(b), result_storage), qbe_offset(b, packet, 8));
    }
    for (i = 0; i < argc; ++i) {
      QbeValue v = actual[i];
      size_t j;
      if (places[i].indirect) {
        const char *copied = qbe_allocate(b, qbe_size(v.type), qbe_alignment(v.type));
        qbe_copy(b, copied, v.operand, qbe_size(v.type));
        qbe_store(b, qbe_pointer_type(b), qbe_value(qbe_pointer_type(b), copied), qbe_offset(b, packet, places[i].pieces[0].packet));
      } else if (!memory_value(v.type)) {
        qbe_store(b, v.type, v, qbe_offset(b, packet, places[i].pieces[0].packet));
      } else for (j = 0; j < places[i].count; ++j) {
        Piece p = places[i].pieces[j];
        qbe_copy(b, qbe_offset(b, packet, p.packet), qbe_offset(b, v.operand, p.value), p.bytes);
      }
    }
    qbe_free(b, places);
    qbe_free(b, actual);
    site = may_throw ? qbe_begin_invoke(b) : 0;
    qbe_runtime_raw(b, "__edg_qbe_call_native", "", 3, (QbeRawArg[]){{"l", image}, {"l", target}, {"l", packet}});
    qbe_end_invoke(b, site);
    if (!*qbe_base_type(b, result_type)) return qbe_value(result_type, "");
    if (result_class.indirect) return qbe_value(result_type, result_storage);
    if (result_class.floating_count) {
      result_storage = qbe_allocate(b, qbe_size(result_type), qbe_alignment(result_type));
      for (i = 0; i < result_class.floating_count; ++i)
        qbe_copy(b, qbe_offset(b, result_storage, result_class.floating[i].offset),
          qbe_offset(b, image, 16 + 16 * i), qbe_size(result_class.floating[i].type));
      return qbe_load(b, result_type, result_storage);
    }
    return qbe_load(b, result_type, image);
  } else {
    QbeBuffer call = {0};
    const char *result = "";
    a_boolean comma = FALSE;
    unsigned site;
    qbe_buffer_printf(b, &call, "call %s(", target);
    for (i = 0; i < argc; ++i) {
      if (rtsp->has_ellipsis && i == count) {
        qbe_buffer_append(b, &call, comma ? ", ..." : "..."); comma = TRUE;
      }
      qbe_buffer_printf(b, &call, "%s%s %s", comma ? ", " : "",
        rtsp->value_returned_as_parameter && i == hidden ? "env" : qbe_abi_type(b, actual[i].type), actual[i].operand);
      comma = TRUE;
    }
    if (rtsp->has_ellipsis && argc == count) qbe_buffer_append(b, &call, comma ? ", ..." : "...");
    qbe_buffer_append(b, &call, ")");
    site = may_throw ? qbe_begin_invoke(b) : 0;
    if (!*qbe_base_type(b, result_type)) qbe_line(b, qbe_buffer_string(&call));
    else result = qbe_emit(b, qbe_abi_type(b, result_type), qbe_buffer_string(&call));
    qbe_end_invoke(b, site);
    qbe_free(b, call.data);
    qbe_free(b, actual);
    return qbe_value(result_type, result);
  }
}

QbeValue qbe_overriding_result(QbeBackend *b) {
  a_variable_ptr p;
  a_routine_ptr target;
  size_t n = 0, i = 0;
  QbeValue *args;
  if (!b->routine_ || !b->routine_->overriding_function_for_wrapper)
    qbe_fail("overriding result outside a lowered thunk");
  for (p = scope_for_routine(b->routine_)->variant.routine.parameters; p; p = p->next) ++n;
  args = qbe_alloc(b, n * sizeof(*args));
  for (p = scope_for_routine(b->routine_)->variant.routine.parameters; p; p = p->next)
    args[i++] = qbe_load(b, p->type, qbe_var_address(b, p));
  target = b->routine_->overriding_function_for_wrapper;
  b->overridingResult_ = qbe_emit_call(b, target, target->type,
    qbe_concat(b, "$", qbe_routine_symbol(b, target), NULL), n, args, !target->never_throws);
  qbe_free(b, args);
  return b->overridingResult_;
}

void qbe_return_value(QbeBackend *b, QbeValue value) {
  a_type_ptr type = qbe_underlying(b->routine_->type)->variant.routine.return_type;
  if (!value.type && *qbe_base_type(b, type)) {
    if (!*b->inlineReturnLabel_ && b->routine_->source_corresp.name && !strcmp(b->routine_->source_corresp.name, "main"))
      qbe_return_value(b, qbe_value(type, "0"));
    else { qbe_line(b, "hlt"); b->terminated_ = TRUE; }
    return;
  }
  if (*b->inlineReturnLabel_) {
    if (*b->inlineReturnSlot_) qbe_store(b, b->inlineReturnType_, qbe_cast(b, value, b->inlineReturnType_), b->inlineReturnSlot_);
    qbe_jump(b, b->inlineReturnLabel_); return;
  }
  if (*b->nativeResult_) {
    if (*qbe_base_type(b, type)) {
      QbeNativeClass c = qbe_native_class(b, type);
      QbeValue v = qbe_cast(b, value, type);
      if (c.indirect) {
        const char *destination = qbe_load(b, qbe_pointer_type(b), qbe_offset(b, b->nativePacket_, 8)).operand;
        qbe_copy(b, destination, v.operand, qbe_size(type));
      } else if (c.floating_count) {
        if (memory_value(type)) {
          size_t i;
          for (i = 0; i < c.floating_count; ++i)
            qbe_copy(b, qbe_offset(b, b->nativeResult_, 16 + 16 * i), qbe_offset(b, v.operand, c.floating[i].offset), qbe_size(c.floating[i].type));
        } else qbe_store(b, type, v, qbe_offset(b, b->nativeResult_, 16));
      } else qbe_store(b, type, v, b->nativeResult_);
    }
    qbe_line(b, "ret");
  } else if (!*qbe_base_type(b, type)) qbe_line(b, "ret");
  else qbe_line(b, qbe_concat(b, "ret ", qbe_cast(b, value, type).operand, NULL));
  b->terminated_ = TRUE;
}

void qbe_emit_routine(QbeBackend *b, a_routine_ptr routine) {
  const char *name = qbe_routine_symbol(b, routine), *body_name, *code, *first;
  a_routine_ptr linkage = routine;
  a_boolean odr_weak, native;
  a_scope_ptr scope;
  a_type_ptr type;
  a_routine_type_supplement_ptr rtsp;
  QbeBuffer signature = {0};
  Registers regs = {0};
  a_variable_ptr p;
  unsigned hidden, i = 0;
  if (routine->is_coroutine)
    qbe_fail("native coroutine suspension lowering is not implemented");
#if IA64_ABI
  if (routine->primary_ctor_or_dtor) linkage = routine->primary_ctor_or_dtor;
  odr_weak = routine->use_comdat || linkage->use_comdat;
#else
  odr_weak = FALSE;
#endif
  odr_weak = odr_weak || (linkage->storage_class != sc_static &&
    (linkage->is_inline || (linkage->is_template_function && !linkage->is_specialized)));
#if GNU_EXTENSIONS_ALLOWED
  if (routine->is_weak || linkage->is_weak) qbe_set_insert(b, &b->weak_, name);
#if GNU_VISIBILITY_ATTRIBUTE_ALLOWED
  if (routine->ELF_visibility == evk_hidden || routine->ELF_visibility == evk_internal) qbe_set_insert(b, &b->hidden_, name);
  if (routine->ELF_visibility == evk_protected) qbe_set_insert(b, &b->protected_, name);
#endif
  if (routine->gnu_extra_info && routine->gnu_extra_info->aliased_routine &&
      (routine->is_gnu_alias || routine->is_weakref)) {
    qbe_alias_set(b, name, (QbeAlias){qbe_routine_symbol(b, routine->gnu_extra_info->aliased_routine), routine->storage_class != sc_static});
    return;
  }
#endif
#if IA64_ABI
  if (routine->is_alias_entry && routine->primary_ctor_or_dtor) {
    if (odr_weak) qbe_set_insert(b, &b->weak_, name);
    qbe_alias_set(b, name, (QbeAlias){qbe_routine_symbol(b, routine->primary_ctor_or_dtor), linkage->storage_class != sc_static});
    return;
  }
#endif
  if (qbe_map_has(&b->defined_, name) || routine->suppress_inline_body || routine->memory_region == NULL_region_number) return;
#if MAINTAIN_NEEDED_FLAGS
  if (!routine->definition_needed) return;
#endif
  scope = routine_scope(routine);
  if (!scope || !scope->assoc_block) qbe_fail("resident routine has no body");
  qbe_set_insert(b, &b->defined_, name);
  if (odr_weak) qbe_set_insert(b, &b->weak_, name);
  b->routine_ = routine; b->scope_ = scope;
  qbe_map_clear(&b->variables_); qbe_map_clear(&b->labels_);
  qbe_buffer_clear(&b->body_); qbe_buffer_clear(&b->entry_);
  b->persistentStackFloor_ = NULL;
  b->nextValue_ = b->nextLabel_ = 0; b->terminated_ = FALSE;
  b->currentLabel_ = ""; b->resultParameter_ = NULL;
  b->overridingResult_ = qbe_void_value();
  b->nativePacket_ = b->nativeResult_ = "";
  b->inlineReturnLabel_ = b->inlineReturnSlot_ = ""; b->inlineReturnType_ = NULL;
  b->inlineForwardRoutine_ = NULL;
  type = qbe_underlying(routine->type);
  rtsp = type->variant.routine.extra_info;
  native = wide_signature(b, type, 0, NULL) || rtsp->has_ellipsis;
  hidden = hidden_result_index(type);
  if (rtsp->has_ellipsis) {
    if (routine->overriding_function_for_wrapper) b->inlineForwardRoutine_ = routine->overriding_function_for_wrapper;
    else if (routine->primary_ctor_or_dtor) b->inlineForwardRoutine_ = routine->primary_ctor_or_dtor;
    else if (routine->special_kind == sfk_lambda_entry_point) b->inlineForwardRoutine_ = routine->variant.lambda_call_operator;
  }
  body_name = name;
  if (native) {
    body_name = qbe_symbol(b, qbe_concat(b, ".L__edg_qbe_native_body_", name, NULL));
    b->nativePacket_ = "%native_packet"; b->nativeResult_ = "%native_result";
    qbe_buffer_printf(b, &signature, "function $%s(l %s, l %s) {\n", body_name, b->nativePacket_, b->nativeResult_);
  } else {
    const char *ret = qbe_abi_type(b, type->variant.routine.return_type);
    if (routine->storage_class != sc_static) qbe_buffer_append(b, &signature, "export ");
    qbe_buffer_append(b, &signature, "function ");
    if (*ret) qbe_buffer_printf(b, &signature, "%s ", ret);
    qbe_buffer_printf(b, &signature, "$%s(", name);
  }
  b->currentFunctionSymbol_ = body_name;
  qbe_label(b, "@start");
  for (p = scope->variant.routine.parameters; p; p = p->next, ++i) {
    a_boolean is_result = rtsp->value_returned_as_parameter && i == hidden;
    const char *slot;
    if (is_result) b->resultParameter_ = p;
    slot = qbe_var_address(b, p);
    if (native) {
      Placement loc = is_result ? hidden_placement() : place(b, p->type, &regs);
      if (loc.indirect) {
        QbeValue pointer = qbe_load(b, qbe_pointer_type(b), qbe_offset(b, b->nativePacket_, loc.pieces[0].packet));
        qbe_copy(b, slot, pointer.operand, qbe_size(p->type));
      } else {
        size_t j;
        for (j = 0; j < loc.count; ++j) {
          Piece part = loc.pieces[j];
          qbe_copy(b, qbe_offset(b, slot, part.value), qbe_offset(b, b->nativePacket_, part.packet), part.bytes);
        }
      }
    } else {
      const char *parameter = qbe_format(b, "%%arg%u", i);
      if (i) qbe_buffer_append(b, &signature, ", ");
      qbe_buffer_printf(b, &signature, "%s %s", is_result ? "env" : qbe_abi_type(b, p->type), parameter);
      qbe_store(b, p->type, qbe_value(p->type, parameter), slot);
    }
  }
  if (native) {
    QbeNativeEntry entry = {name, body_name, regs.stack, routine->storage_class != sc_static};
    b->nativeNamedGP_ = regs.gp; b->nativeNamedFP_ = regs.fp; b->nativeNamedStack_ = regs.stack;
    qbe_vec_push(b, &b->nativeEntries_, entry);
    qbe_zero(b, b->nativeResult_, 80);
  } else {
    qbe_buffer_append(b, &signature, ") {\n");
    b->nativeNamedGP_ = b->nativeNamedFP_ = 0; b->nativeNamedStack_ = 0;
  }
  qbe_begin_function_eh(b, routine, scope);
  qbe_statement(b, scope->assoc_block);
  if (!b->terminated_) {
    a_type_ptr ret = type->variant.routine.return_type;
    if (!*qbe_base_type(b, ret)) qbe_return_value(b, qbe_void_value());
    else if (routine->source_corresp.name && !strcmp(routine->source_corresp.name, "main"))
      qbe_return_value(b, qbe_value(ret, "0"));
    else { qbe_line(b, "hlt"); b->terminated_ = TRUE; }
  }
  qbe_finish_function_eh(b);
  code = qbe_buffer_string(&b->body_);
  first = strchr(code, '\n');
  if (!first) qbe_fail("routine has no QBE entry block");
  qbe_string_vec_push(b, &b->functionDefinitions_,
    qbe_concat(b, qbe_buffer_string(&signature), qbe_string(b, code, (size_t)(first - code + 1)),
      qbe_buffer_string(&b->entry_), first + 1, "}\n", NULL));
  qbe_free(b, signature.data);
#if GNU_EXTENSIONS_ALLOWED
  if (routine->is_initialization_routine || routine->is_finalization_routine) {
    unsigned priority = 65535;
    const char *section, *section_type;
#if GNU_INIT_PRIORITY_ATTRIBUTE_ALLOWED
    if (routine->is_initialization_routine && routine->init_priority) priority = routine->init_priority;
    if (routine->gnu_extra_info) {
      if (routine->is_initialization_routine && routine->has_ctor_priority) priority = routine->gnu_extra_info->ctor_priority;
      if (routine->is_finalization_routine && routine->has_dtor_priority) priority = routine->gnu_extra_info->dtor_priority;
    }
#endif
    section = routine->is_initialization_routine ? ".init_array" : ".fini_array";
    if (priority != 65535) section = qbe_format(b, "%s.%05u", section, priority);
    section_type = routine->is_initialization_routine ? "%init_array" : "%fini_array";
    qbe_add_module_assembly(b, qbe_format(b, ".pushsection %s,\"aw\",%s\n.p2align 3\n.xword %s\n.popsection\n",
      section, section_type, (const char *)qbe_map_get(&b->symbols_, name)));
  }
#endif
  b->routine_ = NULL; b->scope_ = NULL; b->inlineForwardRoutine_ = NULL;
  b->nativePacket_ = b->nativeResult_ = "";
}
