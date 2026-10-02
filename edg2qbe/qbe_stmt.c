#include "qbe_codegen.h"
#include <limits.h>

typedef enum {
  region_try, region_catch, region_internal, region_exception_storage,
  region_inline_frame
} RegionKind;

typedef struct {
  const char *label, *caughtAddress, *exceptionAddress, *pointerCell;
  unsigned selector;
} HandlerTarget;
typedef struct { a_handler_ptr handler; HandlerTarget target; } CatchTarget;
typedef QBE_VEC(CatchTarget) CatchTargetVec;
typedef struct {
  RegionKind kind;
  a_dynamic_init_ptr boundary;
  a_try_supplement_ptr tryBlock;
  a_handler_ptr handler;
  an_expr_node_ptr cleanup;
  const char *storage;
  a_dynamic_init_ptr outerCleanup;
  a_boolean noexceptBoundary, specificationBoundary;
  QbeUnsignedVec allowed;
  a_boolean catchStarted;
  CatchTargetVec catches;
  HandlerTarget caughtTarget;
} Region;
typedef QBE_VEC(Region) RegionVec;
typedef struct {
  size_t boundaryIndex;
  unsigned after;
  QbeUnsignedVec allowed;
  int selector;
} SpecificationTarget;
typedef QBE_VEC(SpecificationTarget) SpecificationTargetVec;
typedef struct {
  unsigned site, pad;
  const char *label, *normal;
  a_dynamic_init_ptr cleanup;
  RegionVec regions;
  a_boolean terminate;
  size_t noexceptIndex;
  a_boolean ignoreFunctionBoundary, noexceptFunction;
  SpecificationTargetVec specifications;
} InvokeTarget;
typedef struct {
  a_dynamic_init_ptr cleanup;
  RegionVec regions;
  a_boolean terminate, ignoreFunctionBoundary;
} FlowState;

struct QbeEHState {
  QbeEHFunction function;
  FlowState flow;
  QbeMap declarationInitializers, handlers, cases, completed;
  QBE_VEC(InvokeTarget) invokes;
  QbeMap invokeIndices;
  QbeStringVec thrownObjects;
  QBE_VEC(FlowState) inlineFlows;
  QBE_VEC(QbeMap) inlineLabels;
  a_boolean noexceptFunction, specificationFunction;
  QbeUnsignedVec functionAllowed;
  uint64_t nextSpecificationOffset;
  a_boolean unwinding;
  unsigned labelEpoch, terminateSelector, terminatePad;
  const char *terminateLabel;
};

static QbeUnsignedVec unsigned_copy(QbeBackend *b, const QbeUnsignedVec *source) {
  QbeUnsignedVec result = {0};
  if (source->len) {
    qbe_vec_reserve(b, (void **)&result.data, &result.cap, source->len,
                    sizeof(*result.data));
    memcpy(result.data, source->data, source->len * sizeof(*result.data));
    result.len = source->len;
  }
  return result;
}

static void region_free(QbeBackend *b, Region *region) {
  qbe_vec_free(b, &region->allowed);
  qbe_vec_free(b, &region->catches);
}

static Region region_copy(QbeBackend *b, const Region *source) {
  Region result = *source;
  result.allowed = unsigned_copy(b, &source->allowed);
  memset(&result.catches, 0, sizeof(result.catches));
  if (source->catches.len) {
    qbe_vec_reserve(b, (void **)&result.catches.data, &result.catches.cap,
                    source->catches.len, sizeof(*result.catches.data));
    memcpy(result.catches.data, source->catches.data,
           source->catches.len * sizeof(*result.catches.data));
    result.catches.len = source->catches.len;
  }
  return result;
}

static void regions_truncate(QbeBackend *b, RegionVec *regions, size_t keep) {
  while (regions->len > keep) region_free(b, &regions->data[--regions->len]);
}

static void regions_free(QbeBackend *b, RegionVec *regions) {
  regions_truncate(b, regions, 0);
  qbe_vec_free(b, regions);
}

static RegionVec regions_copy(QbeBackend *b, const RegionVec *source,
                              size_t count) {
  RegionVec result = {0};
  if (count) {
    qbe_vec_reserve(b, (void **)&result.data, &result.cap, count,
                    sizeof(*result.data));
    for (size_t n = 0; n < count; ++n) {
      result.data[result.len++] = region_copy(b, &source->data[n]);
    }
  }
  return result;
}

static FlowState flow_copy(QbeBackend *b, const FlowState *source) {
  FlowState result = *source;
  result.regions = regions_copy(b, &source->regions, source->regions.len);
  return result;
}

static void flow_free(QbeBackend *b, FlowState *flow) {
  regions_free(b, &flow->regions);
}

static void flow_assign(QbeBackend *b, FlowState *destination,
                        const FlowState *source) {
  if (destination != source) {
    FlowState copy = flow_copy(b, source);
    flow_free(b, destination);
    *destination = copy;
  }
}

static void flow_move(QbeBackend *b, FlowState *destination, FlowState *source) {
  flow_free(b, destination);
  *destination = *source;
  memset(source, 0, sizeof(*source));
}

static FlowState flow_take(FlowState *source) {
  FlowState result = *source;
  memset(source, 0, sizeof(*source));
  return result;
}

static void invoke_free(QbeBackend *b, InvokeTarget *invoke) {
  regions_free(b, &invoke->regions);
  for (size_t n = 0; n < invoke->specifications.len; ++n) {
    qbe_vec_free(b, &invoke->specifications.data[n].allowed);
  }
  qbe_vec_free(b, &invoke->specifications);
}

static void qbe_eh_function_free(QbeBackend *b, QbeEHFunction *function) {
  for (size_t n = 0; n < function->landingpads.len; ++n) {
    QbeEHHandler *pad = &function->landingpads.data[n];
    qbe_vec_free(b, &pad->catches);
    for (size_t k = 0; k < pad->specifications.len; ++k) {
      qbe_vec_free(b, &pad->specifications.data[k].allowed);
    }
    qbe_vec_free(b, &pad->specifications);
  }
  qbe_vec_free(b, &function->landingpads);
  qbe_vec_free(b, &function->sites);
  qbe_vec_free(b, &function->types);
  memset(function, 0, sizeof(*function));
}

void qbe_free_function_eh(QbeBackend *b) {
  QbeEHState *eh = b->eh_;
  if (!eh) return;
  qbe_eh_function_free(b, &eh->function);
  flow_free(b, &eh->flow);
  for (size_t n = 0; n < eh->handlers.len; ++n) {
    qbe_free(b, eh->handlers.data[n].value);
  }
  qbe_map_free(b, &eh->declarationInitializers);
  qbe_map_free(b, &eh->handlers);
  qbe_map_free(b, &eh->cases);
  qbe_map_free(b, &eh->completed);
  for (size_t n = 0; n < eh->invokes.len; ++n) invoke_free(b, &eh->invokes.data[n]);
  qbe_vec_free(b, &eh->invokes);
  qbe_map_free(b, &eh->invokeIndices);
  qbe_vec_free(b, &eh->thrownObjects);
  for (size_t n = 0; n < eh->inlineFlows.len; ++n) flow_free(b, &eh->inlineFlows.data[n]);
  qbe_vec_free(b, &eh->inlineFlows);
  for (size_t n = 0; n < eh->inlineLabels.len; ++n) qbe_map_free(b, &eh->inlineLabels.data[n]);
  qbe_vec_free(b, &eh->inlineLabels);
  qbe_vec_free(b, &eh->functionAllowed);
  qbe_free(b, eh);
  b->eh_ = NULL;
}

static HandlerTarget *handler_target(QbeBackend *b, a_handler_ptr handler) {
  HandlerTarget *target = qbe_map_get(&b->eh_->handlers, handler);
  if (!target) qbe_fail("unknown lowered catch object");
  return target;
}

static void set_handler_target(QbeBackend *b, a_handler_ptr handler,
                               HandlerTarget target) {
  HandlerTarget *stored = qbe_map_get(&b->eh_->handlers, handler);
  if (!stored) {
    stored = qbe_alloc(b, sizeof(*stored));
    qbe_map_put(b, &b->eh_->handlers, handler, stored);
  }
  *stored = target;
}

static const char *label_for(QbeBackend *b, a_label_ptr lab) {
  const char *name = qbe_map_get(&b->labels_, lab);
  if (!name || !*name) {
    name = qbe_new_label(b, "label");
    qbe_map_put(b, &b->labels_, lab, name);
  }
  return name;
}

static const char *completed_slot(QbeBackend *b, a_dynamic_init_ptr init) {
  const char *slot = qbe_map_get(&b->eh_->completed, init);
  if (!slot || !*slot) {
    slot = qbe_allocate(b, 4, 4);
    qbe_map_put(b, &b->eh_->completed, init, slot);
    qbe_buffer_printf(b, &b->entry_, "\tstorew 0, %s\n", slot);
  }
  return slot;
}

static a_dynamic_init_ptr successor(a_dynamic_init_ptr init) {
  if (!init->native_cleanup_address) {
    qbe_fail("lowered cleanup lacks its persistent native destination");
  }
  return init->native_cleanup_next;
}

static const char *condition_operand(QbeBackend *b, QbeValue condition) {
  return strcmp(qbe_base_type(b, condition.type), "w") == 0 ?
         condition.operand : qbe_truth(b, condition);
}

static void raw_invoke(QbeBackend *b, const char *name, size_t argc,
                       const QbeRawArg *arguments) {
  unsigned site = qbe_begin_invoke(b);
  qbe_runtime_raw(b, name, "", argc, arguments);
  qbe_end_invoke(b, site);
}

static void terminate_exception(QbeBackend *b, const char *exception) {
  qbe_runtime_raw(b, "__cxa_begin_catch", "l", 1,
                  (QbeRawArg[]){{"l", exception}});
  qbe_runtime_raw(b, "_ZSt9terminatev", "", 0, NULL);
  qbe_line(b, "hlt");
  b->terminated_ = TRUE;
}

static void cleanup_guard(QbeBackend *b, const char *condition,
                          const char **skip) {
  if (!*skip) *skip = qbe_new_label(b, "cleanup_next");
  const char *perform = qbe_new_label(b, "cleanup_live");
  qbe_branch(b, condition, perform, *skip);
  qbe_label(b, perform);
}

static void destroy_one(QbeBackend *b, a_dynamic_init_ptr init) {
  if (!init->native_cleanup_address) qbe_fail("missing persistent no-table cleanup");
  const char *skip = NULL;
  if (init->native_cleanup_conditional_flag) {
    a_variable_ptr var = init->native_cleanup_conditional_flag;
    cleanup_guard(b, condition_operand(b, qbe_load(b, var->type,
                                                  qbe_var_address(b, var))), &skip);
  }
  if (init->overlaps_temps_in_inner_lifetime ||
      init->destruction_is_for_partially_constructed_aggregate) {
    cleanup_guard(b, qbe_emit(b, "w", qbe_concat(b, "loadw ", completed_slot(b, init), NULL)), &skip);
    qbe_line(b, qbe_concat(b, "storew 0, ", completed_slot(b, init), NULL));
  }
  const char *object = qbe_expression(b, init->native_cleanup_address).operand;
  if (init->is_guard_var_for_local_static_var_init) {
    qbe_runtime_raw(b, "__cxa_guard_abort", "", 1, (QbeRawArg[]){{"l", object}});
  } else if (init->is_vla_deallocation) {
    qbe_vla_deallocate(b, init->variable);
  } else if (init->is_freeing_of_storage_on_exception) {
    if (init->native_cleanup_delete_args) {
      QbeValueVec arguments = {0};
      for (an_expr_node_ptr arg = init->native_cleanup_delete_args; arg; arg = arg->next) {
        qbe_vec_push(b, &arguments, qbe_expression(b, arg));
      }
      a_routine_ptr routine = init->native_cleanup_destructor;
      if (!routine) qbe_fail("lowered deallocation arguments without a delete routine");
      qbe_emit_call(b, routine, routine->type,
                    qbe_concat(b, "$", qbe_routine_symbol(b, routine), NULL),
                    arguments.len, arguments.data, TRUE);
      qbe_vec_free(b, &arguments);
    } else {
      raw_invoke(b, "_ZdlPv", 1, (QbeRawArg[]){{"l", object}});
    }
  } else if (init->native_cleanup_destructor) {
    const char *target = qbe_concat(b, "$", qbe_routine_symbol(b, init->native_cleanup_destructor), NULL);
    QbeRawArg arguments[2] = {{"l", object}};
    size_t argc = 1;
    if (init->native_cleanup_use_delegation_dtor) {
      a_variable_ptr var = init->native_cleanup_delegation_arg;
      if (!var) qbe_fail("delegation cleanup is missing its VTT argument");
      arguments[argc++] = (QbeRawArg){"l", qbe_load(b, var->type, qbe_var_address(b, var)).operand};
    } else if (init->native_cleanup_vtt) {
      arguments[argc++] = (QbeRawArg){"l", qbe_expression(b, init->native_cleanup_vtt).operand};
    }
    a_type_ptr element = init->native_cleanup_element_type;
    a_boolean array = init->native_cleanup_array_count != NULL;
    const char *countValue = NULL;
    if (array) {
      countValue = qbe_cast(b, qbe_expression(b, init->native_cleanup_array_count),
                            qbe_integer_type(b, 64, FALSE)).operand;
      if (!element) qbe_fail("array cleanup is missing its flattened element type");
    }
    if (!array) {
      raw_invoke(b, target, argc, arguments);
    } else {
      const char *cursor = qbe_allocate(b, 8, 8);
      qbe_line(b, qbe_format(b, "storel %s, %s", countValue, cursor));
      const char *test = qbe_new_label(b, "array_cleanup_test");
      const char *body = qbe_new_label(b, "array_cleanup_body");
      const char *done = qbe_new_label(b, "array_cleanup_done");
      qbe_jump(b, test);
      qbe_label(b, test);
      const char *current = qbe_emit(b, "l", qbe_concat(b, "loadl ", cursor, NULL));
      qbe_branch(b, qbe_emit(b, "w", qbe_format(b, "cnel %s, 0", current)), body, done);
      qbe_label(b, body);
      const char *previous = qbe_emit(b, "l", qbe_format(b, "sub %s, 1", current));
      qbe_line(b, qbe_format(b, "storel %s, %s", previous, cursor));
      const char *displacement = qbe_emit(b, "l", qbe_format(b, "mul %s, %llu", previous,
                                           (unsigned long long)qbe_size(element)));
      arguments[0].operand = qbe_emit(b, "l", qbe_format(b, "add %s, %s", object, displacement));
      raw_invoke(b, target, argc, arguments);
      qbe_jump(b, test);
      qbe_label(b, done);
    }
  } else {
    qbe_fail("unrecognized lowered automatic cleanup");
  }
  if (skip) {
    qbe_jump(b, skip);
    qbe_label(b, skip);
  }
}

static void cleanup_to(QbeBackend *b, a_dynamic_init_ptr *current,
                       a_dynamic_init_ptr stop) {
  QbeMap visited = {0};
  while (*current != stop) {
    if (!*current) qbe_fail("exception cleanup boundary is not on the EDG cleanup chain");
    if (!qbe_set_insert(b, &visited, *current)) qbe_fail("cyclic EDG cleanup descriptor chain");
    a_dynamic_init_ptr init = *current;
    *current = successor(init);
    b->eh_->flow.cleanup = *current;
    destroy_one(b, init);
  }
  qbe_map_free(b, &visited);
}

static void unwind_to(QbeBackend *b, const InvokeTarget *invoke, size_t keep,
                      a_boolean escaping) {
  a_dynamic_init_ptr current = invoke->cleanup;
  b->eh_->flow.cleanup = current;
  regions_truncate(b, &b->eh_->flow.regions, 0);
  b->eh_->flow.terminate = TRUE;
  b->eh_->flow.ignoreFunctionBoundary = invoke->ignoreFunctionBoundary;
  b->eh_->unwinding = TRUE;
  qbe_vec_clear(&b->eh_->thrownObjects);
  for (size_t n = 0; n < invoke->regions.len; ++n) {
    const Region *region = &invoke->regions.data[n];
    if (region->kind == region_exception_storage) {
      qbe_vec_push(b, &b->eh_->thrownObjects, region->storage);
    }
    if (region->kind == region_catch) {
      set_handler_target(b, region->handler, region->caughtTarget);
    }
  }
  for (size_t n = invoke->regions.len; n > keep; --n) {
    const Region *region = &invoke->regions.data[n - 1];
    cleanup_to(b, &current, region->boundary);
    if (region->kind == region_catch && region->catchStarted) {
      raw_invoke(b, "__cxa_end_catch", 0, NULL);
    } else if (region->kind == region_internal) {
      qbe_expression(b, region->cleanup);
    } else if (region->kind == region_exception_storage) {
      qbe_runtime_raw(b, "__cxa_free_exception", "", 1,
                      (QbeRawArg[]){{"l", region->storage}});
      QbeStringVec *objects = &b->eh_->thrownObjects;
      if (objects->len && strcmp(objects->data[objects->len - 1], region->storage) == 0) {
        --objects->len;
      }
    } else if (region->kind == region_inline_frame) {
      current = region->outerCleanup;
      b->eh_->flow.cleanup = current;
    }
  }
  if (escaping) cleanup_to(b, &current, NULL);
  b->eh_->flow.cleanup = current;
  b->eh_->unwinding = FALSE;
  qbe_vec_clear(&b->eh_->thrownObjects);
}

static unsigned terminate_selector(QbeBackend *b) {
  if (!b->eh_->terminateSelector) {
    qbe_vec_push(b, &b->eh_->function.types, "");
    b->eh_->terminateSelector = (unsigned)b->eh_->function.types.len;
  }
  return b->eh_->terminateSelector;
}

static a_boolean unsigned_contains(const QbeUnsignedVec *values, unsigned value) {
  for (size_t n = 0; n < values->len; ++n) {
    if (values->data[n] == value) return TRUE;
  }
  return FALSE;
}

static QbeUnsignedVec specification_types(QbeBackend *b,
                                          an_exception_specification_ptr specification) {
  QbeUnsignedVec allowed = {0};
  for (an_exception_specification_type_ptr entry = specification->variant.exception_specification_type_list;
       entry; entry = entry->next) {
    if (entry->redundant) continue;
    a_type_ptr exceptionType = qbe_underlying(entry->type);
    if (exceptionType->kind == tk_pointer && exceptionType->variant.pointer.is_reference) {
      exceptionType = qbe_underlying(exceptionType->variant.pointer.type);
    }
    const char *type = qbe_rtti(b, exceptionType);
    QbeStringVec *types = &b->eh_->function.types;
    size_t found = 0;
    while (found < types->len && strcmp(types->data[found], type) != 0) ++found;
    if (found == types->len) {
      qbe_vec_push(b, types, type);
      qbe_vec_push(b, &allowed, (unsigned)types->len);
    } else {
      unsigned index = (unsigned)found + 1;
      if (!unsigned_contains(&allowed, index)) qbe_vec_push(b, &allowed, index);
    }
  }
  return allowed;
}

static int specification_selector(QbeBackend *b, const QbeUnsignedVec *allowed) {
  uint64_t *offset = &b->eh_->nextSpecificationOffset;
  if (*offset > (uint64_t)(INT32_MAX - 1)) qbe_fail("GNU exception specification table exceeds selector range");
  int selector = -(int)*offset - 1;
  for (size_t n = 0; n < allowed->len; ++n) {
    unsigned type = allowed->data[n];
    do {
      ++*offset;
      type >>= 7;
    } while (type);
  }
  ++*offset; /* The ULEB zero terminating this specification. */
  return selector;
}

static void discard_region(QbeBackend *b, RegionKind kind, const void *identity) {
  RegionVec *regions = &b->eh_->flow.regions;
  for (size_t n = regions->len; n > 0; --n) {
    const Region *region = &regions->data[n - 1];
    if (region->kind == kind &&
        (kind == region_try ? (const void *)region->tryBlock :
                              (const void *)region->handler) == identity) {
      regions_truncate(b, regions, n - 1);
      return;
    }
  }
}

static void end_catch(QbeBackend *b, a_handler_ptr handler) {
  /* Remove this frame before invoking end_catch: destruction must never
     recursively end the same catch again. */
  discard_region(b, region_catch, handler);
  a_boolean saved = b->eh_->flow.terminate;
  b->eh_->flow.terminate = TRUE;
  raw_invoke(b, "__cxa_end_catch", 0, NULL);
  b->eh_->flow.terminate = saved;
}

static void block_declarations(QbeBackend *b, a_scope_ptr scope) {
  if (!scope) return;
  for (a_variable_ptr variable = scope->nonstatic_variables; variable; variable = variable->next) {
    if (variable->init_kind == initk_binding) continue;
    if (variable->is_vla) { qbe_vla_cell(b, variable); continue; }
    const char *destination = qbe_var_address(b, variable);
    if (variable->init_kind == initk_dynamic) {
      a_dynamic_init_ptr init = variable->initializer.dynamic;
      if (init && init->kind == dik_constant && !init->follows_an_exec_statement) {
        qbe_initialize(b, init, destination, variable->type);
        qbe_set_insert(b, &b->eh_->declarationInitializers, init);
      }
    } else if (variable->init_kind == initk_static) {
      qbe_store(b, variable->type, qbe_constant(b, variable->initializer.constant), destination);
    } else if (variable->init_kind == initk_zero) {
      qbe_zero(b, destination, qbe_size(variable->type));
    }
  }
}

void qbe_begin_function_eh(QbeBackend *b, a_routine_ptr routine, a_scope_ptr scope) {
  (void)scope;
  qbe_free_function_eh(b);
  b->eh_ = qbe_alloc(b, sizeof(*b->eh_));
  memset(b->eh_, 0, sizeof(*b->eh_));
  b->eh_->function.name = *b->currentFunctionSymbol_ ?
                          b->currentFunctionSymbol_ : qbe_routine_symbol(b, routine);
  b->eh_->function.personality = qbe_symbol(b, "__gxx_personality_v0");
  a_type_ptr type = qbe_underlying(routine->type);
  an_exception_specification_ptr spec = type->variant.routine.extra_info->exception_specification;
  b->eh_->specificationFunction = !C_mode() && spec && !spec->indeterminate && !spec->throw_any && !spec->is_noexcept;
  if (b->eh_->specificationFunction) b->eh_->functionAllowed = specification_types(b, spec);
  b->eh_->noexceptFunction = !C_mode() && !b->eh_->specificationFunction && (routine->never_throws ||
       (spec && !spec->indeterminate && !spec->throw_any && spec->is_noexcept));
}

void qbe_begin_inline_eh(QbeBackend *b) {
  if (!b->eh_) qbe_fail("inline body outside a function EH context");
  qbe_vec_push(b, &b->eh_->inlineFlows, flow_copy(b, &b->eh_->flow));
  qbe_vec_push(b, &b->eh_->inlineLabels, b->labels_);
  memset(&b->labels_, 0, sizeof(b->labels_));
  Region region = {0};
  region.kind = region_inline_frame;
  region.outerCleanup = b->eh_->flow.cleanup;
  an_exception_specification_ptr spec = qbe_underlying(b->routine_->type)->variant.routine.extra_info->exception_specification;
  region.specificationBoundary = !C_mode() && spec && !spec->indeterminate && !spec->throw_any && !spec->is_noexcept;
  if (region.specificationBoundary) region.allowed = specification_types(b, spec);
  region.noexceptBoundary = !C_mode() && !region.specificationBoundary && (b->routine_->never_throws ||
      (spec && !spec->indeterminate && !spec->throw_any && spec->is_noexcept));
  qbe_vec_push(b, &b->eh_->flow.regions, region);
  b->eh_->flow.cleanup = NULL;
}

void qbe_finish_inline_eh(QbeBackend *b) {
  if (!b->eh_ || !b->eh_->inlineFlows.len) qbe_fail("unbalanced inline body EH context");
  flow_move(b, &b->eh_->flow, &b->eh_->inlineFlows.data[--b->eh_->inlineFlows.len]);
  qbe_map_free(b, &b->labels_);
  b->labels_ = b->eh_->inlineLabels.data[--b->eh_->inlineLabels.len];
  memset(&b->eh_->inlineLabels.data[b->eh_->inlineLabels.len], 0, sizeof(QbeMap));
}

unsigned qbe_begin_invoke(QbeBackend *b) {
  if (!b->eh_) return 0;
  a_boolean regionEffect = FALSE;
  for (size_t n = 0; n < b->eh_->flow.regions.len; ++n) {
    const Region *region = &b->eh_->flow.regions.data[n];
    regionEffect = regionEffect ||
        (region->kind == region_try && region->catches.len) ||
        (region->kind == region_catch && region->catchStarted) ||
        region->kind == region_internal || region->kind == region_exception_storage ||
        (region->kind == region_inline_frame &&
         (region->noexceptBoundary || region->specificationBoundary || region->outerCleanup));
  }
  a_boolean functionBoundary = !b->eh_->flow.ignoreFunctionBoundary &&
                              (b->eh_->noexceptFunction || b->eh_->specificationFunction);
  if (!b->eh_->unwinding && !b->eh_->flow.terminate && !functionBoundary &&
      !b->eh_->flow.cleanup && !regionEffect) return 0;
  InvokeTarget invoke = {0};
  invoke.site = b->nextEH_++;
  invoke.terminate = b->eh_->unwinding || b->eh_->flow.terminate;
  invoke.ignoreFunctionBoundary = b->eh_->flow.ignoreFunctionBoundary;
  invoke.noexceptFunction = b->eh_->noexceptFunction && !invoke.ignoreFunctionBoundary;
  if (invoke.terminate) {
    if (!b->eh_->terminatePad) {
      b->eh_->terminatePad = b->nextEH_++;
      b->eh_->terminateLabel = qbe_new_label(b, "terminate_lpad");
      QbeEHHandler pad = {0};
      pad.id = b->eh_->terminatePad;
      qbe_vec_push(b, &pad.catches, terminate_selector(b));
      qbe_vec_push(b, &b->eh_->function.landingpads, pad);
    }
    invoke.pad = b->eh_->terminatePad;
    invoke.label = b->eh_->terminateLabel;
  } else {
    invoke.pad = b->nextEH_++;
    invoke.label = qbe_new_label(b, "lpad");
    invoke.cleanup = b->eh_->flow.cleanup;
    invoke.regions = regions_copy(b, &b->eh_->flow.regions, b->eh_->flow.regions.len);
    QbeUnsignedVec catches = {0};
    a_boolean all = FALSE;
    for (size_t n = invoke.regions.len; n > 0 && !all; --n) {
      const Region *region = &invoke.regions.data[n - 1];
      if (region->kind == region_inline_frame && region->noexceptBoundary) {
        invoke.noexceptIndex = n;
        qbe_vec_push(b, &catches, terminate_selector(b));
        all = TRUE;
        break;
      }
      if (region->kind == region_inline_frame && region->specificationBoundary) {
        SpecificationTarget specification = {n, (unsigned)catches.len,
                                               unsigned_copy(b, &region->allowed), 0};
        qbe_vec_push(b, &invoke.specifications, specification);
      }
      if (region->kind != region_try) continue;
      for (size_t k = 0; k < region->catches.len; ++k) {
        const CatchTarget *entry = &region->catches.data[k];
        qbe_vec_push(b, &catches, entry->target.selector);
        if (!entry->handler->parameter) { all = TRUE; break; }
      }
    }
    if (!all) {
      if (invoke.noexceptFunction) qbe_vec_push(b, &catches, terminate_selector(b));
      else if (b->eh_->specificationFunction && !invoke.ignoreFunctionBoundary) {
        SpecificationTarget specification = {0, (unsigned)catches.len,
                                               unsigned_copy(b, &b->eh_->functionAllowed), 0};
        qbe_vec_push(b, &invoke.specifications, specification);
      }
    }
    a_boolean cleanup = invoke.cleanup != NULL;
    for (size_t n = 0; n < invoke.regions.len; ++n) {
      cleanup = cleanup || invoke.regions.data[n].kind != region_try;
    }
    if (!catches.len && !invoke.specifications.len && !cleanup) {
      qbe_vec_free(b, &catches);
      invoke_free(b, &invoke);
      return 0;
    }
    QbeEHHandler pad = {0};
    pad.id = invoke.pad;
    pad.catches = catches;
    pad.cleanup = cleanup;
    for (size_t n = 0; n < invoke.specifications.len; ++n) {
      SpecificationTarget *specification = &invoke.specifications.data[n];
      specification->selector = specification_selector(b, &specification->allowed);
      QbeEHSpecification metadata = {specification->after,
                                    unsigned_copy(b, &specification->allowed), specification->selector};
      qbe_vec_push(b, &pad.specifications, metadata);
    }
    qbe_vec_push(b, &b->eh_->function.landingpads, pad);
  }
  invoke.normal = qbe_new_label(b, "invoke_normal");
  QbeEHSite metadata = {invoke.site, invoke.pad};
  qbe_vec_push(b, &b->eh_->function.sites, metadata);
  qbe_map_put(b, &b->eh_->invokeIndices, (const void *)(uintptr_t)invoke.site,
              (const void *)(uintptr_t)(b->eh_->invokes.len + 1));
  qbe_vec_push(b, &b->eh_->invokes, invoke);
  const char *actual = qbe_new_label(b, "invoke_call");
  const char *guard = qbe_runtime_raw(b, qbe_format(b, "$__edg_qbe_guard_%u", invoke.site), "w", 0, NULL);
  qbe_branch(b, guard, invoke.label, actual);
  qbe_label(b, actual);
  qbe_runtime_raw(b, qbe_format(b, "$__edg_qbe_begin_%u", invoke.site), "", 0, NULL);
  return invoke.site;
}

void qbe_end_invoke(QbeBackend *b, unsigned site) {
  if (!site) return;
  size_t index = (size_t)(uintptr_t)qbe_map_get(&b->eh_->invokeIndices,
                                              (const void *)(uintptr_t)site);
  if (!index) qbe_fail("unknown native invoke site");
  const InvokeTarget *invoke = &b->eh_->invokes.data[index - 1];
  const char *edge = qbe_runtime_raw(b, qbe_format(b, "$__edg_qbe_edge_%u", site), "w", 0, NULL);
  qbe_branch(b, edge, invoke->label, invoke->normal);
  qbe_label(b, invoke->normal);
}

void qbe_finish_function_eh(QbeBackend *b) {
  if (!b->eh_ || !b->eh_->function.sites.len) return;
  const char *pairType = ":edg_lpad_pair";
  const char *definition = "type :edg_lpad_pair = { l, w }";
  size_t found = 0;
  while (found < b->typeDefinitions_.len && strcmp(b->typeDefinitions_.data[found], definition) != 0) ++found;
  if (found == b->typeDefinitions_.len) qbe_vec_push(b, &b->typeDefinitions_, definition);
  if (!b->terminated_) qbe_fail("function EH finalization requires a terminated ordinary body");
  for (size_t n = 0; n < b->eh_->invokes.len; ++n) {
    /* Cleanups and unexpected handlers can append sites and relocate invokes.
       This is a borrowed snapshot: its nested buffers are immutable here. */
    InvokeTarget invoke = b->eh_->invokes.data[n];
    if (invoke.terminate) continue;
    qbe_label(b, invoke.label);
    /* The adapter requires this aggregate marker to be the first operation. */
    const char *pair = qbe_emit(b, pairType, qbe_format(b, "call $__edg_qbe_lpad_%u()", invoke.pad));
    const char *exception = qbe_emit(b, "l", qbe_concat(b, "loadl ", pair, NULL));
    const char *selector = qbe_emit(b, "w", qbe_concat(b, "loadw ", qbe_offset(b, pair, 8), NULL));
    for (size_t index = invoke.regions.len; index > 0; --index) {
      const Region *region = &invoke.regions.data[index - 1];
      if (region->kind == region_inline_frame && region->noexceptBoundary) break;
      if (region->kind != region_try) continue;
      for (size_t k = 0; k < region->catches.len; ++k) {
        const CatchTarget *entry = &region->catches.data[k];
        a_handler_ptr handler = entry->handler;
        HandlerTarget target = entry->target;
        const char *selected = qbe_new_label(b, "catch_selected");
        const char *next = qbe_new_label(b, "catch_next");
        qbe_branch(b, qbe_emit(b, "w", qbe_format(b, "ceqw %s, %u", selector, target.selector)), selected, next);
        qbe_label(b, selected);
        unwind_to(b, &invoke, index - 1, FALSE);
        qbe_line(b, qbe_format(b, "storel %s, %s", exception, target.exceptionAddress));
        qbe_jump(b, target.label);
        qbe_label(b, next);
        if (!handler->parameter) break;
      }
    }
    for (size_t k = 0; k < invoke.specifications.len; ++k) {
      const SpecificationTarget *specification = &invoke.specifications.data[k];
      const char *selected = qbe_new_label(b, "unexpected_selected");
      const char *next = qbe_new_label(b, "unexpected_next");
      qbe_branch(b, qbe_emit(b, "w", qbe_format(b, "ceqw %s, %d", selector, specification->selector)), selected, next);
      qbe_label(b, selected);
      size_t keep = specification->boundaryIndex ? specification->boundaryIndex - 1 : 0;
      unwind_to(b, &invoke, keep, specification->boundaryIndex == 0);
      /* The violated specification is exited before unexpected runs.  Its
         replacement exception sees caller boundaries, not this boundary. */
      regions_free(b, &b->eh_->flow.regions);
      b->eh_->flow.regions = regions_copy(b, &invoke.regions, keep);
      b->eh_->flow.terminate = FALSE;
      b->eh_->flow.ignoreFunctionBoundary = invoke.ignoreFunctionBoundary || specification->boundaryIndex == 0;
      raw_invoke(b, "__cxa_call_unexpected", 1, (QbeRawArg[]){{"l", exception}});
      qbe_line(b, "hlt");
      b->terminated_ = TRUE;
      qbe_label(b, next);
    }
    unwind_to(b, &invoke, invoke.noexceptIndex ? invoke.noexceptIndex - 1 : 0,
              invoke.noexceptIndex == 0);
    if (invoke.noexceptFunction || invoke.noexceptIndex) terminate_exception(b, exception);
    else {
      qbe_runtime_raw(b, "_Unwind_Resume", "", 1, (QbeRawArg[]){{"l", exception}});
      qbe_line(b, "hlt");
      b->terminated_ = TRUE;
    }
  }
  if (b->eh_->terminatePad) {
    qbe_label(b, b->eh_->terminateLabel);
    const char *pair = qbe_emit(b, pairType, qbe_format(b, "call $__edg_qbe_lpad_%u()", b->eh_->terminatePad));
    terminate_exception(b, qbe_emit(b, "l", qbe_concat(b, "loadl ", pair, NULL)));
  }
  qbe_vec_push(b, &b->ehFunctions_, b->eh_->function);
  memset(&b->eh_->function, 0, sizeof(b->eh_->function));
}

QbeValue qbe_throw_expression(QbeBackend *b, an_expr_node_ptr node) {
  a_throw_supplement_ptr info = node->variant.throw_info;
  if (!info) {
    unsigned site = qbe_begin_invoke(b);
    qbe_runtime_raw(b, "__cxa_rethrow", "", 0, NULL);
    qbe_end_invoke(b, site);
    qbe_line(b, "hlt");
    b->terminated_ = TRUE;
    return qbe_value(node->type, "0");
  }
  const char *object = qbe_runtime_raw(b, "__cxa_allocate_exception", "l", 1,
      (QbeRawArg[]){{"l", qbe_format(b, "%llu", (unsigned long long)qbe_size(info->type))}});
  FlowState saved = flow_copy(b, &b->eh_->flow);
  Region storage = {0};
  storage.kind = region_exception_storage;
  storage.boundary = saved.cleanup;
  storage.storage = object;
  qbe_vec_push(b, &b->eh_->flow.regions, storage);
  qbe_vec_push(b, &b->eh_->thrownObjects, object);
  if (info->expr) qbe_expression(b, info->expr);
  else if (info->dynamic_init) qbe_initialize(b, info->dynamic_init, object, info->type);
  --b->eh_->thrownObjects.len;
  flow_move(b, &b->eh_->flow, &saved);
  const char *destructor = "0";
  if (info->destructor) destructor = qbe_concat(b, "$", qbe_routine_symbol(b, info->destructor), NULL);
  unsigned site = qbe_begin_invoke(b);
  qbe_runtime_raw(b, "__cxa_throw", "", 3,
                  (QbeRawArg[]){{"l", object}, {"l", qbe_concat(b, "$", qbe_rtti(b, info->type), NULL)},
                                {"l", destructor}});
  qbe_end_invoke(b, site);
  qbe_line(b, "hlt");
  b->terminated_ = TRUE;
  return qbe_value(node->type, "0");
}

void qbe_initialized(QbeBackend *b, a_dynamic_init_ptr initialization) {
  if (!b->eh_ || !initialization || !initialization->overlaps_temps_in_inner_lifetime) return;
  /* Lowering emits an explicit completion marker for overlapping lifetimes;
     an ordinary stmk_init may represent only part of initialization. */
  completed_slot(b, initialization);
}

QbeValue qbe_eh_construct(QbeBackend *b, an_expr_node_ptr node) {
  if (!b->eh_) qbe_fail("lowered EH construct outside a function");
  switch (node->variant.lowered_eh.kind) {
    case leck_caught_object_address: {
      HandlerTarget *target = handler_target(b, node->variant.lowered_eh.variant.caught_object_handler);
      return qbe_load(b, node->type, target->caughtAddress);
    }
    case leck_thrown_object_address:
      if (!b->eh_->thrownObjects.len) qbe_fail("throw destination outside a throw expression");
      return qbe_value(node->type, b->eh_->thrownObjects.data[b->eh_->thrownObjects.len - 1]);
    case leck_cleanup_state:
    case leck_unreachable_cleanup_state: {
      a_dynamic_init_ptr next = node->variant.lowered_eh.variant.cleanup_ptr;
      if (node->variant.lowered_eh.kind == leck_cleanup_state) {
        QbeMap retained = {0}, old = {0};
        for (a_dynamic_init_ptr init = next; init && qbe_set_insert(b, &retained, init); init = successor(init)) {}
        for (a_dynamic_init_ptr init = b->eh_->flow.cleanup; init && qbe_set_insert(b, &old, init);
             init = successor(init)) {
          if (!qbe_map_has(&retained, init) && (init->overlaps_temps_in_inner_lifetime ||
              init->destruction_is_for_partially_constructed_aggregate)) {
            qbe_line(b, qbe_concat(b, "storew 0, ", completed_slot(b, init), NULL));
          }
        }
        if (next && !qbe_map_has(&old, next) && next->destruction_is_for_partially_constructed_aggregate &&
            !next->overlaps_temps_in_inner_lifetime) {
          qbe_line(b, qbe_concat(b, "storew 1, ", completed_slot(b, next), NULL));
        }
        qbe_map_free(b, &retained);
        qbe_map_free(b, &old);
      }
      b->eh_->flow.cleanup = next;
      break;
    }
    case leck_initialization_completed: {
      a_dynamic_init_ptr initialization = node->variant.lowered_eh.variant.dynamic_init;
      if (initialization->overlaps_temps_in_inner_lifetime ||
          initialization->destruction_is_for_partially_constructed_aggregate) {
        qbe_line(b, qbe_concat(b, "storew 1, ", completed_slot(b, initialization), NULL));
      }
      for (an_il_entity_list_entry_ptr retired = initialization->native_cleanup_retired; retired; retired = retired->next) {
        if (retired->entity.kind != iek_dynamic_init) qbe_fail("invalid aggregate cleanup retirement");
        a_dynamic_init_ptr partial = (a_dynamic_init_ptr)retired->entity.ptr;
        qbe_line(b, qbe_concat(b, "storew 0, ", completed_slot(b, partial), NULL));
      }
      break;
    }
    case leck_function_prologue:
      /* Native unwind state belongs to the function, not an EDG EH stack. */
      if (node->variant.lowered_eh.variant.prologue_info->routine != b->routine_) {
        qbe_fail("lowered EH prologue names a different function");
      }
      break;
    case leck_function_epilogue:
      if (node->variant.lowered_eh.variant.epilogue_routine != b->routine_) qbe_fail("invalid lowered EH epilogue");
      b->eh_->flow.cleanup = NULL;
      break;
    case leck_catch_epilogue:
      end_catch(b, node->variant.lowered_eh.variant.epilogue_handler);
      break;
    case leck_try_epilogue:
      discard_region(b, region_try, node->variant.lowered_eh.variant.epilogue_try_block);
      break;
    case leck_exception_caught: {
      /* GNU catch-by-value construction precedes begin_catch, preserving
         the uncaught-exception count seen by its copy constructor. */
      a_boolean found = FALSE;
      for (size_t n = b->eh_->flow.regions.len; n > 0; --n) {
        Region *region = &b->eh_->flow.regions.data[n - 1];
        if (region->kind != region_catch) continue;
        HandlerTarget *target = handler_target(b, region->handler);
        const char *exception = qbe_emit(b, "l", qbe_concat(b, "loadl ", target->exceptionAddress, NULL));
        qbe_runtime_raw(b, "__cxa_begin_catch", "l", 1, (QbeRawArg[]){{"l", exception}});
        region->catchStarted = TRUE;
        found = TRUE;
        break;
      }
      if (!found) qbe_fail("catch completion marker outside a handler");
      b->eh_->flow.terminate = FALSE;
      break;
    }
    case leck_exception_started:
      b->eh_->flow.terminate = TRUE;
      break;
    case leck_internal_try: {
      an_expr_node_ptr first = node->variant.lowered_eh.variant.try_and_catch_expr;
      if (!first || !first->next) qbe_fail("invalid lowered internal try expression");
      FlowState saved = flow_copy(b, &b->eh_->flow);
      Region region = {0};
      region.kind = region_internal;
      region.boundary = saved.cleanup;
      region.cleanup = first->next;
      qbe_vec_push(b, &b->eh_->flow.regions, region);
      qbe_expression(b, first);
      flow_move(b, &b->eh_->flow, &saved);
      break;
    }
    default: qbe_fail("unknown lowered exception construct");
  }
  return qbe_value(node->type, "0");
}

QbeValue qbe_lifetime_expression(QbeBackend *b, an_expr_node_ptr node) {
  if (!node || node->kind != enk_object_lifetime) qbe_fail("invalid lowered lifetime expression");
  /* EDG already inserted state transitions and normal destruction in this
     operand; destroying the lifetime list again would double-destroy temps. */
  return qbe_expression(b, node->variant.object_lifetime.expr);
}

void qbe_statement_list(QbeBackend *b, a_statement_ptr first) {
  for (a_statement_ptr stmt = first; stmt; stmt = stmt->next) qbe_statement(b, stmt);
}

const char *qbe_lifetime_address(QbeBackend *b, an_expr_node_ptr node) {
  if (!node || node->kind != enk_object_lifetime) qbe_fail("invalid lowered lifetime address");
  return qbe_address(b, node->variant.object_lifetime.expr);
}

void qbe_statement(QbeBackend *b, a_statement_ptr stmt) {
  if (!stmt) return;
  a_boolean openedDead = b->terminated_ && stmt->kind != stmk_label && stmt->kind != stmk_switch_case;
  unsigned labelEpoch = b->eh_->labelEpoch;
  if (openedDead) qbe_label(b, qbe_new_label(b, "unreachable"));
  switch (stmt->kind) {
    case stmk_empty: break;
    case stmk_expr:
    case stmk_coroutine_return:
      if (stmt->expr) qbe_expression(b, stmt->expr);
      break;
    case stmk_block: {
      a_scope_ptr saved = b->scope_;
      a_block_ptr extra = stmt->variant.block.extra_info;
      if (extra && extra->assoc_scope) b->scope_ = extra->assoc_scope;
      if ((extra && extra->assoc_scope) || (b->scope_ && b->scope_->assoc_block == stmt)) {
        block_declarations(b, b->scope_);
      }
      qbe_statement_list(b, stmt->variant.block.statements);
      b->scope_ = saved;
      break;
    }
    case stmk_constexpr_if: {
      a_constexpr_if_ptr info = stmt->variant.constexpr_if;
      qbe_statement(b, info->value ? info->then_statement : info->else_statement);
      break;
    }
    case stmk_if_consteval:
      qbe_statement(b, stmt->variant.if_stmt.else_statement);
      break;
    case stmk_if_not_consteval:
      qbe_statement(b, stmt->variant.if_stmt.then_statement);
      break;
    case stmk_if: {
      const char *yes = qbe_new_label(b, "if_yes");
      const char *no = qbe_new_label(b, "if_no");
      const char *done = qbe_new_label(b, "if_done");
      QbeValue condition = qbe_expression(b, stmt->expr);
      a_boolean conditionTerminated = b->terminated_;
      if (conditionTerminated) qbe_label(b, qbe_new_label(b, "condition_dead"));
      FlowState before = flow_copy(b, &b->eh_->flow);
      qbe_branch(b, condition_operand(b, condition), yes, no);
      qbe_label(b, yes);
      qbe_statement(b, stmt->variant.if_stmt.then_statement);
      FlowState after = flow_take(&b->eh_->flow);
      a_boolean thenTerminated = b->terminated_;
      qbe_jump(b, done);
      flow_assign(b, &b->eh_->flow, &before);
      qbe_label(b, no);
      qbe_statement(b, stmt->variant.if_stmt.else_statement);
      FlowState elseAfter = flow_take(&b->eh_->flow);
      a_boolean elseTerminated = b->terminated_;
      qbe_jump(b, done);
      qbe_label(b, done);
      if (thenTerminated && elseTerminated) flow_move(b, &b->eh_->flow, &before);
      else if (thenTerminated) flow_move(b, &b->eh_->flow, &elseAfter);
      else flow_move(b, &b->eh_->flow, &after);
      flow_free(b, &before);
      flow_free(b, &after);
      flow_free(b, &elseAfter);
      if (conditionTerminated || (thenTerminated && elseTerminated)) {
        qbe_line(b, "hlt");
        b->terminated_ = TRUE;
      }
      break;
    }
    case stmk_while:
    case stmk_end_test_while:
    case stmk_for: {
      a_for_loop_ptr info = stmt->kind == stmk_for ? stmt->variant.for_loop.extra_info : NULL;
      if (info) qbe_statement_list(b, info->initialization);
      FlowState before = flow_copy(b, &b->eh_->flow);
      const char *test = qbe_new_label(b, "loop_test");
      const char *body = qbe_new_label(b, "loop_body");
      const char *increment = qbe_new_label(b, "loop_increment");
      const char *done = qbe_new_label(b, "loop_done");
      QbeStringPair loop = {done, stmt->kind == stmk_for ? increment : test};
      qbe_vec_push(b, &b->loops_, loop);
      qbe_jump(b, stmt->kind == stmk_end_test_while ? body : test);
      qbe_label(b, test);
      if (stmt->expr) {
        QbeValue condition = qbe_expression(b, stmt->expr);
        if (b->terminated_) qbe_label(b, qbe_new_label(b, "condition_dead"));
        qbe_branch(b, condition_operand(b, condition), body, done);
      } else qbe_jump(b, body);
      qbe_label(b, body);
      qbe_statement(b, stmt->kind == stmk_for ? stmt->variant.for_loop.statement : stmt->variant.loop_statement);
      qbe_jump(b, increment);
      qbe_label(b, increment);
      flow_assign(b, &b->eh_->flow, &before);
      if (info && info->increment) qbe_expression(b, info->increment);
      qbe_jump(b, test);
      qbe_label(b, done);
      --b->loops_.len;
      flow_move(b, &b->eh_->flow, &before);
      break;
    }
    case stmk_range_based_for: {
      a_range_based_for_loop_ptr info = stmt->variant.range_based_for_loop.extra_info;
      qbe_statement_list(b, info->initialization);
      a_variable_ptr variables[3] = {info->range, info->begin, info->end};
      for (size_t n = 0; n < 3; ++n) {
        a_variable_ptr variable = variables[n];
        if (variable && variable->init_kind == initk_dynamic) qbe_initialize(b, variable->initializer.dynamic, "", NULL);
      }
      FlowState before = flow_copy(b, &b->eh_->flow);
      const char *test = qbe_new_label(b, "range_test");
      const char *body = qbe_new_label(b, "range_body");
      const char *increment = qbe_new_label(b, "range_increment");
      const char *done = qbe_new_label(b, "range_done");
      QbeStringPair loop = {done, increment};
      qbe_vec_push(b, &b->loops_, loop);
      qbe_jump(b, test);
      qbe_label(b, test);
      QbeValue condition = qbe_expression(b, info->ne_call_expr);
      if (b->terminated_) qbe_label(b, qbe_new_label(b, "condition_dead"));
      qbe_branch(b, condition_operand(b, condition), body, done);
      qbe_label(b, body);
      if (info->iterator && info->iterator->init_kind == initk_dynamic) {
        qbe_initialize(b, info->iterator->initializer.dynamic, "", NULL);
      }
      qbe_statement(b, stmt->variant.range_based_for_loop.statement);
      qbe_jump(b, increment);
      qbe_label(b, increment);
      flow_assign(b, &b->eh_->flow, &before);
      qbe_expression(b, info->incr_call_expr);
      qbe_jump(b, test);
      qbe_label(b, done);
      --b->loops_.len;
      flow_move(b, &b->eh_->flow, &before);
      break;
    }
    case stmk_goto:
      qbe_jump(b, label_for(b, stmt->variant.label.ptr));
      break;
    case stmk_label: {
      const char *target = label_for(b, stmt->variant.label.ptr);
      qbe_jump(b, target);
      qbe_label(b, target);
      ++b->eh_->labelEpoch;
      break;
    }
    case stmk_return: {
      QbeValue result = qbe_void_value();
      if (stmt->variant.return_dynamic_init) {
        a_type_ptr resultType = qbe_underlying(b->routine_->type)->variant.routine.return_type;
        if (b->resultParameter_) {
          a_type_ptr pointer = qbe_underlying(b->resultParameter_->type);
          if (pointer->kind != tk_pointer) qbe_fail("return object parameter is not a pointer");
          resultType = pointer->variant.pointer.type;
        }
        const char *destination = b->resultParameter_ ?
                   qbe_load(b, b->resultParameter_->type, qbe_var_address(b, b->resultParameter_)).operand :
                   qbe_allocate(b, qbe_size(resultType), qbe_alignment(resultType));
        qbe_initialize(b, stmt->variant.return_dynamic_init, destination, resultType);
        result = qbe_value(resultType, destination);
      } else if (stmt->expr) result = qbe_expression(b, stmt->expr);
      if (!b->terminated_) qbe_return_value(b, result);
      break;
    }
    case stmk_switch: {
      a_switch_stmt_descr_ptr info = stmt->variant.switch_stmt.extra_info;
      const char *done = qbe_new_label(b, "switch_done");
      FlowState before = flow_copy(b, &b->eh_->flow);
      for (a_switch_case_entry_ptr item = info->cases; item; item = item->next) {
        qbe_map_put(b, &b->eh_->cases, item, qbe_new_label(b, "switch_case"));
      }
      QbeValue selector = qbe_expression(b, stmt->expr);
      if (b->terminated_) qbe_label(b, qbe_new_label(b, "switch_dead"));
      const char *type = qbe_base_type(b, selector.type);
      for (a_switch_case_entry_ptr item = info->cases; item; item = item->next) {
        if (!item->case_value) continue;
        QbeValue value = qbe_cast(b, qbe_constant(b, item->case_value), selector.type);
        const char *match;
#if GNU_EXTENSIONS_ALLOWED
        if (item->range_end) {
          QbeValue end = qbe_cast(b, qbe_constant(b, item->range_end), selector.type);
          const char *low = qbe_emit(b, "w", qbe_format(b, "%s%s %s, %s",
                            qbe_signed_integer(selector.type) ? "csge" : "cuge", type, selector.operand, value.operand));
          const char *high = qbe_emit(b, "w", qbe_format(b, "%s%s %s, %s",
                             qbe_signed_integer(selector.type) ? "csle" : "cule", type, selector.operand, end.operand));
          match = qbe_emit(b, "w", qbe_format(b, "and %s, %s", low, high));
        } else
#endif
        match = qbe_emit(b, "w", qbe_format(b, "ceq%s %s, %s", type, selector.operand, value.operand));
        const char *next = qbe_new_label(b, "switch_test");
        qbe_branch(b, match, qbe_map_get(&b->eh_->cases, item), next);
        qbe_label(b, next);
      }
      qbe_jump(b, info->default_case ? qbe_map_get(&b->eh_->cases, info->default_case) : done);
      QbeStringPair loop = {done, b->loops_.len ? b->loops_.data[b->loops_.len - 1].second : ""};
      qbe_vec_push(b, &b->loops_, loop);
      qbe_statement(b, stmt->variant.switch_stmt.body_statement);
      qbe_jump(b, done);
      qbe_label(b, done);
      --b->loops_.len;
      flow_move(b, &b->eh_->flow, &before);
      break;
    }
    case stmk_switch_case: {
      const char *target = qbe_map_get(&b->eh_->cases, stmt->variant.switch_case.extra_info);
      if (!target) qbe_fail("unknown lowered switch case");
      qbe_jump(b, target);
      qbe_label(b, target);
      ++b->eh_->labelEpoch;
      break;
    }
    case stmk_init:
      if (!qbe_map_has(&b->eh_->declarationInitializers, stmt->variant.dynamic_init)) {
        qbe_initialize(b, stmt->variant.dynamic_init, "", NULL);
      }
      break;
    case stmk_decl:
      /* Lowering places executable initializers in stmk_init.  Materialize
         automatic declarations, including declarations following labels. */
      for (an_il_entity_list_entry_ptr entity = stmt->variant.decl.entities; entity; entity = entity->next) {
        if (entity->entity.kind == iek_variable) {
          a_variable_ptr var = (a_variable_ptr)entity->entity.ptr;
          if (var->storage_class == sc_auto || var->storage_class == sc_register) qbe_var_address(b, var);
        }
      }
      break;
    case stmk_try_block: {
      a_try_supplement_ptr info = stmt->variant.try_block;
      FlowState before = flow_copy(b, &b->eh_->flow);
      const char *done = qbe_new_label(b, "try_done");
      for (a_handler_ptr handler = info->handlers; handler; handler = handler->next) {
        HandlerTarget target = {0};
        target.label = qbe_new_label(b, "catch_body");
        if (handler->parameter) target.caughtAddress = qbe_allocate(b, 8, 8);
        target.exceptionAddress = qbe_allocate(b, 8, 8);
        if (handler->native_catch_pointer) target.pointerCell = qbe_allocate(b, 8, 8);
        qbe_vec_push(b, &b->eh_->function.types, handler->parameter ?
             (handler->typeinfo_var ? qbe_variable_symbol(b, handler->typeinfo_var) :
                                     qbe_rtti(b, handler->parameter->type)) : "");
        target.selector = (unsigned)b->eh_->function.types.len;
        set_handler_target(b, handler, target);
      }
      Region region = {0};
      region.kind = region_try;
      region.tryBlock = info;
      region.boundary = before.cleanup;
      for (a_handler_ptr handler = info->handlers; handler; handler = handler->next) {
        CatchTarget entry = {handler, *handler_target(b, handler)};
        qbe_vec_push(b, &region.catches, entry);
      }
      qbe_vec_push(b, &b->eh_->flow.regions, region);
      qbe_statement(b, info->statement);
      qbe_jump(b, done);
      for (a_handler_ptr handler = info->handlers; handler; handler = handler->next) {
        flow_assign(b, &b->eh_->flow, &before);
        HandlerTarget target = *handler_target(b, handler);
        qbe_label(b, target.label);
        const char *exception = qbe_emit(b, "l", qbe_concat(b, "loadl ", target.exceptionAddress, NULL));
        const char *adjusted = handler->parameter ?
            qbe_runtime_raw(b, "__cxa_get_exception_ptr", "l", 1, (QbeRawArg[]){{"l", exception}}) : "0";
        if (handler->native_catch_pointer) {
          qbe_line(b, qbe_format(b, "storel %s, %s", adjusted, target.pointerCell));
          adjusted = target.pointerCell;
        }
        if (handler->parameter) qbe_line(b, qbe_format(b, "storel %s, %s", adjusted, target.caughtAddress));
        Region catchRegion = {0};
        catchRegion.kind = region_catch;
        catchRegion.handler = handler;
        catchRegion.boundary = before.cleanup;
        catchRegion.caughtTarget = target;
        qbe_vec_push(b, &b->eh_->flow.regions, catchRegion);
        b->eh_->flow.terminate = TRUE;
        qbe_statement(b, handler->statement);
        if (!b->terminated_) {
          a_boolean active = FALSE;
          for (size_t n = 0; n < b->eh_->flow.regions.len; ++n) {
            const Region *frame = &b->eh_->flow.regions.data[n];
            active = active || (frame->kind == region_catch && frame->handler == handler);
          }
          if (active) {
            if (info->is_function_try_block &&
                (b->routine_->special_kind == sfk_constructor || b->routine_->special_kind == sfk_destructor)) {
              unsigned site = qbe_begin_invoke(b);
              qbe_runtime_raw(b, "__cxa_rethrow", "", 0, NULL);
              qbe_end_invoke(b, site);
              qbe_line(b, "hlt");
              b->terminated_ = TRUE;
            } else end_catch(b, handler);
          }
          qbe_jump(b, done);
        }
      }
      qbe_label(b, done);
      flow_move(b, &b->eh_->flow, &before);
      break;
    }
    case stmk_stmt_expr_result: {
      if (!b->statementResults_.len || !b->statementResultDestinations_.len) {
        qbe_fail("statement-expression result outside a statement expression");
      }
      QbeValue value;
      const char *destination = b->statementResultDestinations_.data[b->statementResultDestinations_.len - 1];
      size_t resultIndex = b->statementResults_.len - 1;
      a_type_ptr resultType = b->statementResults_.data[resultIndex].type;
      if (stmt->variant.stmt_expr_result.dynamic_init) {
        qbe_initialize(b, stmt->variant.stmt_expr_result.dynamic_init, destination, NULL);
        value = qbe_value(resultType, destination);
      } else {
        value = qbe_expression(b, stmt->expr);
        qbe_store(b, resultType, value, destination);
      }
      b->statementResults_.data[resultIndex] = qbe_value(resultType, destination);
      break;
    }
    case stmk_set_vla_size:
      if (stmt->variant.vla_dimension->dimension_expr) qbe_expression(b, stmt->variant.vla_dimension->dimension_expr);
      break;
    case stmk_vla_decl:
      if (!stmt->variant.vla.is_typedef_decl) {
        a_variable_ptr variable = stmt->variant.vla.variant.variable;
        if (variable->is_vla) qbe_vla_allocate(b, variable);
        else qbe_var_address(b, variable);
      }
      break;
    case stmk_coroutine:
      if (!stmt->variant.coroutine.descr->body_generated) qbe_fail("unlowered coroutine body");
      break;
    case stmk_asm: {
      an_asm_entry_ptr assembly = stmt->variant.asm_entry;
#if GNU_EXTENSIONS_ALLOWED
      if (assembly->operands || assembly->labels) {
        qbe_fail("QBE cannot represent constrained inline assembly operands or asm goto");
      }
#endif
      a_constant_ptr text = assembly->asm_string;
      if (!text || text->kind != ck_string) qbe_fail("invalid inline assembly string");
      const char *source = (const char *)text->variant.string.value;
      size_t length = text->variant.string.length ? text->variant.string.length - 1 : 0;
      char *code = qbe_alloc(b, length + 1);
      size_t used = 0;
      for (size_t n = 0; n < length; ++n) {
        code[used++] = source[n];
#if GNU_EXTENSIONS_ALLOWED
        if (assembly->gnu_asm_form && source[n] == '%' && n + 1 < length && source[n + 1] == '%') ++n;
#endif
      }
      code[used] = '\0';
      const char *name = qbe_format(b, "__edg_qbe_asm_%u", b->nextEH_++);
      /* A real opaque call even for an empty barrier prevents QBE moving
         memory accesses across its unknown memory effects. */
      qbe_add_module_assembly(b, qbe_format(b,
          ".text\n.p2align 2\n.type %s, %%function\n%s"
          ":\nstp x29, x30, [sp, #-160]!\nmov x29, sp\n"
          "stp x19, x20, [sp, #16]\nstp x21, x22, [sp, #32]\n"
          "stp x23, x24, [sp, #48]\nstp x25, x26, [sp, #64]\n"
          "stp x27, x28, [sp, #80]\nstp d8, d9, [sp, #96]\n"
          "stp d10, d11, [sp, #112]\nstp d12, d13, [sp, #128]\n"
          "stp d14, d15, [sp, #144]\n%s"
          "\nldp d14, d15, [sp, #144]\nldp d12, d13, [sp, #128]\n"
          "ldp d10, d11, [sp, #112]\nldp d8, d9, [sp, #96]\n"
          "ldp x27, x28, [sp, #80]\nldp x25, x26, [sp, #64]\n"
          "ldp x23, x24, [sp, #48]\nldp x21, x22, [sp, #32]\n"
          "ldp x19, x20, [sp, #16]\nldp x29, x30, [sp], #160\nret\n"
          ".size %s, .-%s\n", name, name, code, name, name));
      qbe_runtime_raw(b, name, "", 0, NULL);
      break;
    }
#if GNU_EXTENSIONS_ALLOWED
    case stmk_assigned_goto:
      qbe_fail("QBE cannot represent a GNU computed goto");
#endif
    default: qbe_fail(qbe_format(b, "unhandled lowered statement kind %u", (unsigned)stmt->kind));
  }
  if (openedDead && b->eh_->labelEpoch == labelEpoch && !b->terminated_) {
    qbe_line(b, "hlt");
    b->terminated_ = TRUE;
  }
}
