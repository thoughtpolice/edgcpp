#include "qbe_codegen.h"
#include "il.h"
#include <stdlib.h>
#include <string.h>

/* All formatted operands belong to the backend's module arena. */
#define F(...) qbe_format(b, __VA_ARGS__)
#define E(t, ...) qbe_emit(b, (t), F(__VA_ARGS__))
#define L(...) qbe_line(b, F(__VA_ARGS__))
#define RAW(name, result, ...) qbe_runtime_raw(b, (name), (result), sizeof((QbeRawArg[]){__VA_ARGS__}) / sizeof(QbeRawArg), (QbeRawArg[]){__VA_ARGS__})
#define EQ(a, s) (strcmp((a), (s)) == 0)

static a_boolean floating(a_type_ptr t) { return qbe_underlying(t)->kind == tk_float; }
static a_boolean wide(a_type_ptr t) {
  t = qbe_underlying(t);
  return (t->kind == tk_integer || t->kind == tk_float) && qbe_size(t) > 8;
}
static void continue_unreachable(QbeBackend *b) {
  if (b->terminated_) qbe_label(b, qbe_new_label(b, "dead"));
}
const char *qbe_truth(QbeBackend *b, QbeValue v) {
  continue_unreachable(b);
  if (wide(v.type)) {
    const char *z = qbe_allocate(b, 16, 16);
    qbe_zero(b, z, 16);
    return RAW(floating(v.type) ? "__edg_qbe_tf_ne" : "__edg_qbe_i128_ne", "w", {"l", v.operand}, {"l", z});
  }
  if (qbe_narrow_float(v.type))
    return E("w", "cnew %s, 0", E("w", "and %s, 32767", v.operand));
  const char *t = qbe_base_type(b, v.type);
  return E("w", "cne%s %s, %s", t, v.operand, EQ(t, "s") ? "s_0" : EQ(t, "d") ? "d_0" : "0");
}

static a_field_ptr selected_field(an_expr_node_ptr n) {
  if (n->kind != enk_operation) return NULL;
  an_expr_operator_kind op = n->variant.operation.kind;
  if (op == eok_lvalue_cast || op == eok_lvalue_adjust || op == eok_parens)
    return selected_field(n->variant.operation.operands);
  if (op != eok_dot_field && op != eok_points_to_field) return NULL;
  return n->variant.operation.operands->next->variant.field.ptr;
}
static QbeValue read_bits(QbeBackend *b, a_type_ptr t, const char *p, a_field_ptr f) {
  unsigned count = (f->offset_bit_remainder + f->bit_size + 7) / 8;
  if (f->bit_size > 64) {
    if (f->bit_size > 128) qbe_fail("bit field exceeds native integer precision");
    a_field low = *f, high = *f;
    low.bit_size = 64; low.bit_field_is_signed = FALSE;
    high.bit_size = f->bit_size - 64;
    const char *out = qbe_allocate(b, 16, 16);
    QbeValue lo = read_bits(b, qbe_integer_type(b, 64, FALSE), p, &low);
    QbeValue hi = read_bits(b, qbe_integer_type(b, 64, f->bit_field_is_signed), qbe_offset(b, p, 8), &high);
    L("storel %s, %s", lo.operand, out);
    L("storel %s, %s", hi.operand, qbe_offset(b, out, 8));
    return qbe_cast(b, qbe_value(qbe_integer_type(b, 128, f->bit_field_is_signed), out), t);
  }
  const char *value = "0";
  for (unsigned i = 0; i < count; ++i) {
    const char *byte = E("l", "loadub %s", qbe_offset(b, p, i));
    unsigned low = i == 0 ? f->offset_bit_remainder : 0;
    unsigned high = f->offset_bit_remainder + f->bit_size - i * 8;
    if (high > 8) high = 8;
    if (low) byte = E("l", "shr %s, %u", byte, low);
    unsigned bits = high - low;
    if (bits != 8) byte = E("l", "and %s, %u", byte, (1u << bits) - 1);
    unsigned shift = i * 8 + low - f->offset_bit_remainder;
    if (shift) byte = E("l", "shl %s, %u", byte, shift);
    value = E("l", "or %s, %s", value, byte);
  }
  if (f->bit_field_is_signed && f->bit_size < 64) {
    value = E("l", "shl %s, %u", value, (unsigned)(64 - f->bit_size));
    value = E("l", "sar %s, %u", value, (unsigned)(64 - f->bit_size));
  }
  return qbe_cast(b, qbe_value(qbe_integer_type(b, 64, f->bit_field_is_signed), value), t);
}
static QbeValue write_bits(QbeBackend *b, a_type_ptr t, QbeValue v, const char *p, a_field_ptr f) {
  if (f->bit_size > 64) {
    if (f->bit_size > 128) qbe_fail("bit field exceeds native integer precision");
    QbeValue full = qbe_cast(b, v, qbe_integer_type(b, 128, FALSE));
    a_field low = *f, high = *f;
    low.bit_size = 64; low.bit_field_is_signed = FALSE;
    high.bit_size = f->bit_size - 64;
    a_type_ptr i64 = qbe_integer_type(b, 64, FALSE);
    write_bits(b, i64, qbe_load(b, i64, full.operand), p, &low);
    write_bits(b, i64, qbe_load(b, i64, qbe_offset(b, full.operand, 8)), qbe_offset(b, p, 8), &high);
    return read_bits(b, t, p, f);
  }
  QbeValue x = qbe_cast(b, v, qbe_integer_type(b, 64, FALSE));
  unsigned count = (f->offset_bit_remainder + f->bit_size + 7) / 8;
  for (unsigned i = 0; i < count; ++i) {
    unsigned low = i == 0 ? f->offset_bit_remainder : 0;
    unsigned high = f->offset_bit_remainder + f->bit_size - i * 8;
    if (high > 8) high = 8;
    unsigned mask = ((1u << (high - low)) - 1) << low;
    unsigned shift = i * 8 + low - f->offset_bit_remainder;
    const char *byte = x.operand;
    if (shift) byte = E("l", "shr %s, %u", byte, shift);
    if (low) byte = E("l", "shl %s, %u", byte, low);
    byte = E("w", "and %s, %u", byte, mask);
    const char *loc = qbe_offset(b, p, i);
    const char *old = E("w", "loadub %s", loc);
    old = E("w", "and %s, %u", old, 255 ^ mask);
    byte = E("w", "or %s, %s", old, byte);
    L("storeb %s, %s", byte, loc);
  }
  return read_bits(b, t, p, f);
}
static QbeValue binary(QbeBackend *b, an_expr_operator_kind op, QbeValue x, QbeValue y, a_type_ptr result) {
  if (!x.type || !y.type)
    qbe_fail(F("valueless operand for native binary operator %d in %s", (int)op,
               b->routine_ && b->routine_->source_corresp.name ? b->routine_->source_corresp.name : "<global>"));
  continue_unreachable(b);
  if (qbe_underlying(x.type)->kind == tk_vector) {
    a_type_ptr xe = qbe_underlying(x.type)->variant.vector.element_type;
    a_type_ptr ye = qbe_underlying(y.type)->kind == tk_vector ? qbe_underlying(y.type)->variant.vector.element_type : y.type;
    a_type_ptr re = qbe_underlying(result)->variant.vector.element_type;
    a_boolean comparison = FALSE;
    switch (op) {
      case eok_vector_eq: op = eok_eq; comparison = TRUE; break;
      case eok_vector_ne: op = eok_ne; comparison = TRUE; break;
      case eok_vector_lt: op = eok_lt; comparison = TRUE; break;
      case eok_vector_le: op = eok_le; comparison = TRUE; break;
      case eok_vector_gt: op = eok_gt; comparison = TRUE; break;
      case eok_vector_ge: op = eok_ge; comparison = TRUE; break;
      case eok_eq: case eok_ne: case eok_lt: case eok_le: case eok_gt: case eok_ge: comparison = TRUE; break;
      default: break;
    }
    const char *out = qbe_allocate(b, qbe_size(result), qbe_alignment(result));
    uint64_t count = qbe_size(x.type) / qbe_size(xe);
    for (uint64_t i = 0; i < count; ++i) {
      QbeValue xv = qbe_load(b, xe, qbe_offset(b, x.operand, i * qbe_size(xe)));
      QbeValue yv = qbe_underlying(y.type)->kind == tk_vector ? qbe_load(b, ye, qbe_offset(b, y.operand, i * qbe_size(ye))) : y;
      QbeValue lane = binary(b, op, xv, yv, re);
      if (comparison) lane = qbe_value(re, E(qbe_base_type(b, re), "sub 0, %s", lane.operand));
      qbe_store(b, re, lane, qbe_offset(b, out, i * qbe_size(re)));
    }
    return qbe_value(result, out);
  }
  a_type_ptr xt = qbe_underlying(x.type), yt = qbe_underlying(y.type);
  a_boolean xn = xt->kind == tk_integer || xt->kind == tk_enum || xt->kind == tk_float;
  a_boolean yn = yt->kind == tk_integer || yt->kind == tk_enum || yt->kind == tk_float;
  if (xn && yn) {
    a_boolean shift = op == eok_shiftl || op == eok_shiftr;
    if (shift) {
      if (qbe_width(x.type) < 32) x = qbe_cast(b, x, qbe_integer_type(b, 32, TRUE));
    } else if (floating(x.type) || floating(y.type)) {
      a_type_ptr common = floating(x.type) && floating(y.type) ? (qbe_size(x.type) >= qbe_size(y.type) ? x.type : y.type) : floating(x.type) ? x.type : y.type;
      x = qbe_cast(b, x, common); y = qbe_cast(b, y, common);
    } else {
      unsigned xb = qbe_width(x.type), yb = qbe_width(y.type);
      if (xb < 32) xb = 32;
      if (yb < 32) yb = 32;
      a_boolean xs = qbe_width(x.type) < 32 || qbe_signed_integer(x.type);
      a_boolean ys = qbe_width(y.type) < 32 || qbe_signed_integer(y.type);
      a_boolean sign = xb == yb ? xs && ys : xb > yb ? xs : ys;
      a_type_ptr common = qbe_integer_type(b, xb > yb ? xb : yb, sign);
      x = qbe_cast(b, x, common); y = qbe_cast(b, y, common);
    }
  }
  /* Binary64 prevents an intermediate binary32 rounding of narrow results. */
  if (qbe_narrow_float(x.type)) {
    x = qbe_cast(b, x, float_type(fk_double));
    y = qbe_cast(b, y, float_type(fk_double));
  }
  a_boolean fp = floating(x.type), sign = qbe_signed_integer(x.type);
  const char *instruction = NULL;
  a_boolean comparison = FALSE;
  switch (op) {
    case eok_add: instruction = "add"; break;
    case eok_subtract: instruction = "sub"; break;
    case eok_multiply: instruction = "mul"; break;
    case eok_divide: instruction = fp || sign ? "div" : "udiv"; break;
    case eok_remainder: instruction = sign ? "rem" : "urem"; break;
    case eok_shiftl: instruction = "shl"; break;
    case eok_shiftr: instruction = sign ? "sar" : "shr"; break;
    case eok_and: instruction = "and"; break;
    case eok_or: instruction = "or"; break;
    case eok_xor: instruction = "xor"; break;
    case eok_eq: instruction = "eq"; comparison = TRUE; break;
    case eok_ne: instruction = "ne"; comparison = TRUE; break;
    case eok_lt: instruction = "lt"; comparison = TRUE; break;
    case eok_le: instruction = "le"; comparison = TRUE; break;
    case eok_gt: instruction = "gt"; comparison = TRUE; break;
    case eok_ge: instruction = "ge"; comparison = TRUE; break;
    default: qbe_fail(F("unhandled binary operator %d", (int)op));
  }
  if (wide(x.type)) {
    const char *name = fp ? "__edg_qbe_tf_" : "__edg_qbe_i128_";
    if (comparison) {
      if (!fp && !sign && op != eok_eq && op != eok_ne) instruction = F("u%s", instruction);
      return qbe_cast(b, qbe_value(qbe_integer_type(b, 32, FALSE), RAW(F("%s%s", name, instruction), "w", {"l", x.operand}, {"l", y.operand})), result);
    }
    const char *out = qbe_allocate(b, 16, 16);
    if (op == eok_shiftl || op == eok_shiftr) {
      y = qbe_cast(b, y, qbe_integer_type(b, 32, FALSE));
      RAW(F("%s%s", name, instruction), "", {"l", out}, {"l", x.operand}, {"w", y.operand});
    } else RAW(F("%s%s", name, instruction), "", {"l", out}, {"l", x.operand}, {"l", y.operand});
    return qbe_value(result, out);
  }
  const char *t = qbe_base_type(b, x.type);
  if (comparison) {
    if (!fp && op != eok_eq && op != eok_ne) instruction = F("%s%s", sign ? "s" : "u", instruction);
    return qbe_cast(b, qbe_value(qbe_integer_type(b, 32, FALSE), E("w", "c%s%s %s, %s", instruction, t, x.operand, y.operand)), result);
  }
  return qbe_cast(b, qbe_value(x.type, E(t, "%s %s, %s", instruction, x.operand, y.operand)), result);
}

QbeValue qbe_cast(QbeBackend *b, QbeValue v, a_type_ptr type) {
  if (!type || qbe_underlying(type)->kind == tk_void) return qbe_value(type, "");
  continue_unreachable(b);
  if (!v.type)
    qbe_fail(F("casting a void expression in %s to type kind %d", b->routine_ && b->routine_->source_corresp.name ? b->routine_->source_corresp.name : "<global>", (int)qbe_underlying(type)->kind));
  a_type_ptr src = qbe_underlying(v.type), dst = qbe_underlying(type);
  if (dst->kind == tk_integer && dst->variant.integer.bool_type)
    return qbe_value(type, qbe_truth(b, v));
  a_boolean sf = floating(src), df = floating(dst), sw = wide(src), dw = wide(dst);
  a_boolean sn = qbe_narrow_float(src), dn = qbe_narrow_float(dst);
  if (sn && dn && (src->variant.float_kind == fk_std_bfloat16) == (dst->variant.float_kind == fk_std_bfloat16))
    return qbe_value(type, v.operand);
  if (sn) {
    QbeValue expanded = qbe_value(float_type(fk_float), RAW("__edg_qbe_narrow_to_float", "s", {"w", v.operand}, {"w", src->variant.float_kind == fk_std_bfloat16 ? "1" : "0"}));
    return qbe_cast(b, expanded, type);
  }
  if (dn) {
    a_boolean bf = dst->variant.float_kind == fk_std_bfloat16;
    if (sf && sw) return qbe_runtime(b, bf ? "__trunctfbf2" : "__trunctfhf2", type, 1, &v, FALSE);
    if (!sf && sw) return qbe_runtime(b, F("%s%s", qbe_signed_integer(src) ? "__floatti" : "__floatunti", bf ? "bf" : "hf"), type, 1, &v, FALSE);
    if (!sf && bf) {
      QbeValue integer = qbe_cast(b, v, qbe_integer_type(b, 64, qbe_signed_integer(src)));
      return qbe_runtime(b, qbe_signed_integer(src) ? "__floatdibf" : "__floatundibf", type, 1, &integer, FALSE);
    }
    /* Integers inside binary16's finite boundary are exact in binary64. */
    QbeValue expanded = qbe_cast(b, v, float_type(fk_double));
    return qbe_value(type, RAW("__edg_qbe_narrow_from_double", "w", {"d", expanded.operand}, {"w", bf ? "1" : "0"}));
  }
  if ((qbe_aggregate(src) && !sw) || (qbe_aggregate(dst) && !dw)) {
    uint64_t source_size = qbe_size(src), destination_size = qbe_size(dst);
    if (source_size != destination_size)
      qbe_fail(F("aggregate cast in %s changes storage size: %s kind %d size %llu to %s kind %d size %llu",
                 b->routine_ && b->routine_->source_corresp.name ? b->routine_->source_corresp.name : "<global>",
                 src->source_corresp.name ? src->source_corresp.name : "<anonymous>",
                 (int)src->kind, (unsigned long long)source_size,
                 dst->source_corresp.name ? dst->source_corresp.name : "<anonymous>",
                 (int)dst->kind, (unsigned long long)destination_size));
    return qbe_value(type, v.operand);
  }
  if (sw || dw) {
    if (sf == df && sw && dw) return qbe_value(type, v.operand);
    if (dw) {
      const char *out = qbe_allocate(b, 16, 16), *name;
      QbeRawArg args[3] = {{"l", out}};
      size_t argc = 1;
      if (sw) {
        name = df ? (qbe_signed_integer(src) ? "__edg_qbe_tf_from_i128" : "__edg_qbe_tf_from_u128") : (qbe_signed_integer(dst) ? "__edg_qbe_i128_from_tf" : "__edg_qbe_u128_from_tf");
        args[argc++] = (QbeRawArg){"l", v.operand};
      } else if (sf) {
        const char *fptype = qbe_base_type(b, src);
        name = df ? (EQ(fptype, "s") ? "__edg_qbe_tf_from_float" : "__edg_qbe_tf_from_double") : (EQ(fptype, "s") ? "__edg_qbe_i128_from_float" : "__edg_qbe_i128_from_double");
        args[argc++] = (QbeRawArg){fptype, v.operand};
        if (!df) args[argc++] = (QbeRawArg){"w", qbe_signed_integer(dst) ? "0" : "1"};
      } else {
        QbeValue i = qbe_cast(b, v, qbe_integer_type(b, 64, qbe_signed_integer(src)));
        name = F("%s%s", df ? "__edg_qbe_tf_from_" : "__edg_qbe_i128_from_", qbe_signed_integer(src) ? "signed" : "unsigned");
        args[argc++] = (QbeRawArg){"l", i.operand};
      }
      qbe_runtime_raw(b, name, "", argc, args);
      return qbe_value(type, out);
    }
    if (sf) {
      const char *name = df ? (EQ(qbe_base_type(b, type), "s") ? "__edg_qbe_tf_to_float" : "__edg_qbe_tf_to_double") : qbe_signed_integer(dst) ? "__edg_qbe_tf_to_signed" : "__edg_qbe_tf_to_unsigned";
      QbeValue x = qbe_value(df ? type : qbe_integer_type(b, 64, qbe_signed_integer(dst)), RAW(name, df ? qbe_base_type(b, type) : "l", {"l", v.operand}));
      return df ? x : qbe_cast(b, x, type);
    }
    if (df) {
      const char *fp_type = qbe_base_type(b, type);
      return qbe_value(type, RAW(EQ(fp_type, "s") ? "__edg_qbe_i128_to_float" : "__edg_qbe_i128_to_double", fp_type, {"l", v.operand}, {"w", qbe_signed_integer(src) ? "0" : "1"}));
    }
    return qbe_cast(b, qbe_load(b, qbe_integer_type(b, 64, qbe_signed_integer(dst)), v.operand), type);
  }
  const char *s = qbe_base_type(b, src), *d = qbe_base_type(b, dst), *op = v.operand;
  if (sf && df) {
    if (!EQ(s, d)) op = E(d, "%s%s", EQ(s, "s") ? "exts " : "truncd ", op);
  } else if (sf) {
    op = E(d, "%s%s%s", s, qbe_signed_integer(dst) ? "tosi " : "toui ", op);
  } else if (df) {
    op = E(d, "%s%s%s", qbe_signed_integer(src) ? "s" : "u", EQ(s, "l") ? "ltof " : "wtof ", op);
  } else {
    if (EQ(s, "w") && EQ(d, "l")) op = E("l", "%s%s", qbe_signed_integer(src) ? "extsw " : "extuw ", op);
    if (qbe_width(dst) == 8 || qbe_width(dst) == 16) {
      op = E("w", "%s%s", qbe_width(dst) == 8 ? (qbe_signed_integer(dst) ? "extsb " : "extub ") : (qbe_signed_integer(dst) ? "extsh " : "extuh "), op);
    } else if (EQ(s, "l") && EQ(d, "w")) op = E("w", "copy %s", op);
  }
  if (sf && !df && (qbe_width(dst) == 8 || qbe_width(dst) == 16))
    op = E("w", "%s%s", qbe_width(dst) == 8 ? (qbe_signed_integer(dst) ? "extsb " : "extub ") : (qbe_signed_integer(dst) ? "extsh " : "extuh "), op);
  return qbe_value(type, op);
}

const char *qbe_address(QbeBackend *b, an_expr_node_ptr n) {
  if (!n) qbe_fail("null lvalue expression");
  continue_unreachable(b);
  switch (n->kind) {
    case enk_variable: return qbe_var_address(b, n->variant.variable.ptr);
    case enk_routine: return F("$%s", qbe_routine_symbol(b, n->variant.routine.ptr));
    case enk_constant: {
      QbeValue v = qbe_constant(b, n->variant.constant.ptr);
      if (qbe_aggregate(n->type) || wide(n->type)) return v.operand;
      const char *p = qbe_allocate(b, qbe_size(n->type), qbe_alignment(n->type));
      qbe_store(b, n->type, v, p); return p;
    }
    case enk_temp_init: case enk_lambda: {
      a_dynamic_init_ptr di = n->variant.init.dynamic_init;
      const char *p = di->variable ? qbe_var_address(b, di->variable) : qbe_allocate(b, qbe_size(n->type), qbe_alignment(n->type));
      qbe_initialize(b, di, p, n->type); return p;
    }
    case enk_object_lifetime: return qbe_lifetime_address(b, n);
    case enk_c11_generic: return qbe_address(b, n->variant.c11_generic.result);
    case enk_operation: {
      an_expr_operator_kind op = n->variant.operation.kind;
      an_expr_node_ptr a = n->variant.operation.operands, c = a->next;
      switch (op) {
        case eok_indirect: return qbe_expression(b, a).operand;
        case eok_lvalue_cast: case eok_lvalue_adjust: case eok_parens: return qbe_address(b, a);
        case eok_dot_field: case eok_points_to_field: {
          const char *p = op == eok_dot_field ? qbe_address(b, a) : qbe_expression(b, a).operand;
          return qbe_offset(b, p, c->variant.field.ptr->offset);
        }
        case eok_gnu_min: case eok_gnu_max: {
          const char *lhs, *rhs;
          if (n->variant.operation.eval_right_to_left) { rhs = qbe_address(b, c); lhs = qbe_address(b, a); }
          else { lhs = qbe_address(b, a); rhs = qbe_address(b, c); }
          QbeValue left = qbe_load(b, a->type, lhs), right = qbe_load(b, c->type, rhs);
          QbeValue test = binary(b, op == eok_gnu_min ? eok_lt : eok_gt, left, right, qbe_integer_type(b, 32, FALSE));
          const char *slot = qbe_allocate(b, 8, 8), *yes = qbe_new_label(b, "b"), *no = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b");
          qbe_branch(b, test.operand, yes, no); qbe_label(b, yes); L("storel %s, %s", lhs, slot); qbe_jump(b, end);
          qbe_label(b, no); L("storel %s, %s", rhs, slot); qbe_jump(b, end); qbe_label(b, end);
          return E("l", "loadl %s", slot);
        }
        case eok_subscript: {
          an_expr_node_ptr p = n->variant.operation.pointer_operand_is_second ? c : a;
          QbeValue first, second;
          if (n->variant.operation.eval_right_to_left) { second = qbe_expression(b, c); first = qbe_expression(b, a); }
          else { first = qbe_expression(b, a); second = qbe_expression(b, c); }
          QbeValue pv = p == a ? first : second, iv = p == a ? second : first;
          iv = qbe_cast(b, iv, qbe_integer_type(b, 64, qbe_signed_integer(iv.type)));
          const char *stride = qbe_runtime_size(b, qbe_underlying(p->type)->variant.pointer.type);
          if (EQ(stride, "0")) stride = "1";
          const char *ix = iv.operand;
          if (!EQ(stride, "1")) ix = E("l", "mul %s, %s", ix, stride);
          return E("l", "add %s, %s", pv.operand, ix);
        }
        case eok_vector_subscript: {
          QbeValue v = qbe_expression(b, a), index = qbe_cast(b, qbe_expression(b, c), qbe_integer_type(b, 64, FALSE));
          const char *displacement = E("l", "mul %s, %llu", index.operand, (unsigned long long)qbe_size(n->type));
          return E("l", "add %s, %s", v.operand, displacement);
        }
        case eok_comma: case eok_dot_static: case eok_points_to_static:
          qbe_expression(b, a); return qbe_address(b, c);
        case eok_question: {
          const char *slot = qbe_allocate(b, 8, 8), *yes = qbe_new_label(b, "b"), *no = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b");
          qbe_branch(b, qbe_truth(b, qbe_expression(b, a)), yes, no);
          qbe_label(b, yes); L("storel %s, %s", qbe_address(b, c), slot); qbe_jump(b, end);
          qbe_label(b, no); L("storel %s, %s", qbe_address(b, c->next), slot); qbe_jump(b, end);
          qbe_label(b, end); return E("l", "loadl %s", slot);
        }
        default: break;
      }
      break;
    }
    default: break;
  }
  if (qbe_aggregate(n->type)) return qbe_expression(b, n).operand;
  qbe_fail(F("expression has no address, node %d", (int)n->kind));
}

QbeValue qbe_expression(QbeBackend *b, an_expr_node_ptr n) {
  if (!n) return qbe_void_value();
  continue_unreachable(b);
  switch (n->kind) {
    case enk_address_of_ellipsis:
      if (b->inlineForwardRoutine_ && b->nativePacket_[0]) return qbe_value(n->type, b->nativePacket_);
      qbe_fail("ellipsis forwarding outside native saved argument packet");
    case enk_statement: {
      a_type_ptr t = n->type;
      a_boolean isvoid = qbe_underlying(t)->kind == tk_void;
      const char *p = isvoid ? "" : qbe_allocate(b, qbe_size(t), qbe_alignment(t));
      qbe_value_vec_push(b, &b->statementResults_, qbe_value(t, ""));
      qbe_string_vec_push(b, &b->statementResultDestinations_, p);
      qbe_statement(b, n->variant.statement);
      QbeValue result = b->statementResults_.data[--b->statementResults_.len];
      --b->statementResultDestinations_.len;
      if (isvoid || b->terminated_) return qbe_value(t, "");
      if (!result.operand[0]) qbe_fail("statement expression lacks result");
      return qbe_load(b, t, p);
    }
    case enk_constant: return qbe_cast(b, qbe_constant(b, n->variant.constant.ptr), n->type);
    case enk_variable: return qbe_load(b, n->type, qbe_var_address(b, n->variant.variable.ptr));
    case enk_routine: return qbe_value(n->type, F("$%s", qbe_routine_symbol(b, n->variant.routine.ptr)));
    case enk_operation: return qbe_operation(b, n);
    case enk_c11_generic: return qbe_expression(b, n->variant.c11_generic.result);
    case enk_temp_init: case enk_lambda: return qbe_load(b, n->type, qbe_address(b, n));
    case enk_throw: return qbe_throw_expression(b, n);
    case enk_lowered_eh_construct: return qbe_eh_construct(b, n);
    case enk_object_lifetime: return qbe_lifetime_expression(b, n);
#if VLA_DEALLOCATIONS_IN_IL
    case enk_vla_dealloc:
      qbe_vla_deallocate(b, n->variant.vla_variable);
      return qbe_value(n->type, "");
#endif
    case enk_sizeof: case enk_datasizeof: case enk_alignof: {
      a_type_ptr t = n->variant.sizeof_info.is_type ? n->variant.sizeof_info.variant.type : n->variant.sizeof_info.variant.expr->type;
      if (n->kind != enk_alignof)
        return qbe_value(n->type, qbe_runtime_size(b, t));
      return qbe_value(n->type, F("%u", qbe_alignment(t)));
    }
    case enk_builtin_choose_expr: {
      an_expr_node_ptr a = n->variant.builtin_choose_expr.operands->next;
      return qbe_expression(b, n->variant.builtin_choose_expr.choose_first ? a : a->next);
    }
    case enk_builtin_operation: {
      an_expr_node_ptr a = n->variant.builtin_operation.operands;
      if (n->variant.builtin_operation.kind == bok_builtin_bit_cast) {
        QbeValue v = qbe_expression(b, a->next);
        if (qbe_size(v.type) != qbe_size(n->type)) qbe_fail("bit_cast storage sizes disagree");
        const char *p = qbe_allocate(b, qbe_size(n->type), qbe_alignment(n->type));
        qbe_store(b, v.type, v, p); return qbe_load(b, n->type, p);
      }
      if (n->variant.builtin_operation.kind == bok_offsetof && a->next->kind == enk_field)
        return qbe_value(n->type, F("%llu", (unsigned long long)a->next->variant.field.ptr->offset));
      qbe_fail(F("unfolded builtin operation %d", (int)n->variant.builtin_operation.kind));
    }
    case enk_result_of_overriding_function: return qbe_overriding_result(b);
    default: qbe_fail(F("unsupported lowered expression node %d", (int)n->kind));
  }
}

static QbeValue vector_lane(QbeBackend *b, QbeValue v, uint64_t i) {
  a_type_ptr vt = qbe_underlying(v.type);
  return vt->kind == tk_vector ? qbe_load(b, vt->variant.vector.element_type, qbe_offset(b, v.operand, i * qbe_size(vt->variant.vector.element_type))) : v;
}
static QbeValue call_argument(QbeBackend *b, an_expr_node_ptr arg) {
  QbeValue value = qbe_expression(b, arg);
  if (qbe_aggregate(value.type) || wide(value.type)) {
    const char *p = qbe_allocate(b, qbe_size(value.type), qbe_alignment(value.type));
    qbe_copy(b, p, value.operand, qbe_size(value.type)); value.operand = p;
  }
  return value;
}

QbeValue qbe_operation(QbeBackend *b, an_expr_node_ptr n) {
  an_expr_operator_kind op = n->variant.operation.kind;
  an_expr_node_ptr a = n->variant.operation.operands, c = a ? a->next : NULL;
  if (op == eok_vector_fill) {
    QbeValue v = qbe_expression(b, a);
    a_type_ptr element = qbe_underlying(n->type)->variant.vector.element_type;
    v = qbe_cast(b, v, element);
    const char *out = qbe_allocate(b, qbe_size(n->type), qbe_alignment(n->type));
    for (uint64_t i = 0; i < qbe_size(n->type); i += qbe_size(element)) qbe_store(b, element, v, qbe_offset(b, out, i));
    return qbe_value(n->type, out);
  }
  if (op == eok_vector_not || op == eok_vector_land || op == eok_vector_lor || op == eok_vector_question) {
    QbeValue x = qbe_expression(b, a), y = qbe_void_value(), z = qbe_void_value();
    if (c) y = qbe_expression(b, c);
    if (c && c->next) z = qbe_expression(b, c->next);
    a_type_ptr element = qbe_underlying(n->type)->variant.vector.element_type;
    const char *out = qbe_allocate(b, qbe_size(n->type), qbe_alignment(n->type));
    for (uint64_t i = 0; i < qbe_size(n->type) / qbe_size(element); ++i) {
      const char *cond = qbe_truth(b, vector_lane(b, x, i));
      if (op == eok_vector_question) {
        const char *yes = qbe_new_label(b, "b"), *no = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b"), *dst = qbe_offset(b, out, i * qbe_size(element));
        qbe_branch(b, cond, yes, no); qbe_label(b, yes); qbe_store(b, element, vector_lane(b, y, i), dst); qbe_jump(b, end);
        qbe_label(b, no); qbe_store(b, element, vector_lane(b, z, i), dst); qbe_jump(b, end); qbe_label(b, end); continue;
      }
      if (op == eok_vector_not) cond = E("w", "ceqw %s, 0", cond);
      else cond = E("w", "%s %s, %s", op == eok_vector_land ? "and" : "or", cond, qbe_truth(b, vector_lane(b, y, i)));
      QbeValue result = qbe_cast(b, qbe_value(qbe_integer_type(b, 32, FALSE), cond), element);
      result.operand = E(qbe_base_type(b, element), "sub 0, %s", result.operand);
      qbe_store(b, element, result, qbe_offset(b, out, i * qbe_size(element)));
    }
    return qbe_value(n->type, out);
  }
  if (a && qbe_underlying(a->type)->kind == tk_vector && (op == eok_negate || op == eok_complement)) {
    QbeValue v = qbe_expression(b, a);
    a_type_ptr element = qbe_underlying(v.type)->variant.vector.element_type;
    const char *out = qbe_allocate(b, qbe_size(n->type), qbe_alignment(n->type));
    for (uint64_t i = 0; i < qbe_size(v.type); i += qbe_size(element)) {
      QbeValue lane = qbe_load(b, element, qbe_offset(b, v.operand, i));
      const char *t = qbe_base_type(b, element);
      lane.operand = op == eok_complement ? E(t, "xor %s, -1", lane.operand) : qbe_narrow_float(element) ? E(t, "xor %s, 32768", lane.operand) : floating(element) ? E(t, "neg %s", lane.operand) : E(t, "sub 0, %s", lane.operand);
      qbe_store(b, element, lane, qbe_offset(b, out, i));
    }
    return qbe_value(n->type, out);
  }
  switch (op) {
    case eok_address_of: case eok_array_to_pointer: return qbe_value(n->type, qbe_address(b, a));
    case eok_indirect: case eok_subscript: case eok_vector_subscript: case eok_dot_field: case eok_points_to_field:
    case eok_lvalue_adjust: case eok_lvalue_cast: {
      const char *p = qbe_address(b, n); a_field_ptr f = selected_field(n);
      if (op == eok_indirect) {
        a_type_ptr pointee = qbe_underlying(qbe_underlying(a->type)->variant.pointer.type);
        /* n->type already includes implicit function-to-pointer conversion. */
        if (pointee->kind == tk_routine) return qbe_value(n->type, p);
      }
      return f && f->is_bit_field ? read_bits(b, n->type, p, f) : qbe_load(b, n->type, p);
    }
    case eok_cast: case eok_bool_cast: return qbe_cast(b, qbe_expression(b, a), n->type);
    case eok_class_rvalue_adjust:
    case eok_parens: case eok_unary_plus: return qbe_cast(b, qbe_expression(b, a), n->type);
    case eok_comma: case eok_dot_static: case eok_points_to_static:
      qbe_expression(b, a); return qbe_cast(b, qbe_expression(b, c), n->type);
    case eok_dot_vacuous_destructor_call: case eok_points_to_vacuous_destructor_call:
      qbe_expression(b, a); return qbe_value(n->type, "");
    case eok_gnu_min: case eok_gnu_max: {
      QbeValue x, y;
      if (n->variant.operation.eval_right_to_left) { y = qbe_expression(b, c); x = qbe_expression(b, a); }
      else { x = qbe_expression(b, a); y = qbe_expression(b, c); }
      QbeValue cond = binary(b, op == eok_gnu_min ? eok_lt : eok_gt, x, y, qbe_integer_type(b, 32, FALSE));
      const char *p = qbe_allocate(b, qbe_size(n->type), qbe_alignment(n->type)), *yes = qbe_new_label(b, "b"), *no = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b");
      qbe_branch(b, cond.operand, yes, no); qbe_label(b, yes); qbe_store(b, n->type, x, p); qbe_jump(b, end);
      qbe_label(b, no); qbe_store(b, n->type, y, p); qbe_jump(b, end); qbe_label(b, end); return qbe_load(b, n->type, p);
    }
    case eok_land: case eok_lor: case eok_question: {
      const char *yes = qbe_new_label(b, "b"), *no = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b");
      a_boolean logical = op != eok_question;
      a_type_ptr t = logical ? qbe_integer_type(b, 32, FALSE) : n->type;
      a_boolean isvoid = qbe_underlying(t)->kind == tk_void;
      const char *slot = isvoid ? "" : qbe_allocate(b, qbe_size(t), qbe_alignment(t));
      qbe_branch(b, qbe_truth(b, qbe_expression(b, a)), yes, no);
      qbe_label(b, yes);
      QbeValue y = logical ? (op == eok_lor ? qbe_value(t, "1") : qbe_value(t, qbe_truth(b, qbe_expression(b, c)))) : qbe_expression(b, c);
      if (!isvoid && !b->terminated_) qbe_store(b, t, y, slot);
      qbe_jump(b, end);
      qbe_label(b, no);
      QbeValue z = logical ? (op == eok_land ? qbe_value(t, "0") : qbe_value(t, qbe_truth(b, qbe_expression(b, c)))) : qbe_expression(b, c->next);
      if (!isvoid && !b->terminated_) qbe_store(b, t, z, slot);
      qbe_jump(b, end); qbe_label(b, end);
      return isvoid ? qbe_value(n->type, "") : qbe_cast(b, qbe_load(b, t, slot), n->type);
    }
    case eok_not: return qbe_value(n->type, E("w", "ceqw %s, 0", qbe_truth(b, qbe_expression(b, a))));
    case eok_negate: case eok_complement: {
      QbeValue v = qbe_expression(b, a);
      if (wide(v.type)) {
        if (op == eok_complement) {
          const char *p = qbe_allocate(b, 16, 16); L("storel -1, %s", p); L("storel -1, %s", qbe_offset(b, p, 8));
          return binary(b, eok_xor, v, qbe_value(v.type, p), n->type);
        }
        const char *p = qbe_allocate(b, 16, 16);
        RAW(floating(v.type) ? "__edg_qbe_tf_neg" : "__edg_qbe_i128_neg", "", {"l", p}, {"l", v.operand});
        return qbe_value(n->type, p);
      }
      const char *t = qbe_base_type(b, v.type);
      if (op == eok_complement) return qbe_cast(b, qbe_value(v.type, E(t, "xor %s, -1", v.operand)), n->type);
      if (qbe_narrow_float(v.type)) return qbe_cast(b, qbe_value(v.type, E("w", "xor %s, 32768", v.operand)), n->type);
      return qbe_cast(b, qbe_value(v.type, floating(v.type) ? E(t, "neg %s", v.operand) : E(t, "sub 0, %s", v.operand)), n->type);
    }
    case eok_call: {
      if (a->kind == enk_routine && a->variant.routine.ptr->source_corresp.name) {
        const char *intrinsic = a->variant.routine.ptr->source_corresp.name;
        if (EQ(intrinsic, "__builtin_constant_p")) return qbe_value(n->type, "0");
        if (EQ(intrinsic, "__builtin_object_size")) {
          QbeValue values[] = {qbe_value(qbe_pointer_type(b), "0"), qbe_expression(b, c->next)};
          QbeValue result; qbe_builtin(b, a->variant.routine.ptr, 2, values, &result); return qbe_cast(b, result, n->type);
        }
      }
      size_t argc = 0;
      for (an_expr_node_ptr p = c; p; p = p->next) ++argc;
      an_expr_node_ptr *nodes = argc && n->variant.operation.eval_right_to_left ? qbe_alloc(b, argc * sizeof(*nodes)) : NULL;
      QbeValue *args = argc ? qbe_alloc(b, argc * sizeof(*args)) : NULL;
      size_t i = 0;
      QbeValue target;
      if (n->variant.operation.eval_right_to_left) {
        for (an_expr_node_ptr p = c; p; p = p->next) nodes[i++] = p;
        for (i = argc; i; --i) args[i - 1] = call_argument(b, nodes[i - 1]);
        target = qbe_expression(b, a);
      } else {
        target = qbe_expression(b, a);
        for (an_expr_node_ptr p = c; p; p = p->next) args[i++] = call_argument(b, p);
      }
      QbeValue result;
      a_boolean intrinsic = a->kind == enk_routine && qbe_builtin(b, a->variant.routine.ptr, argc, args, &result);
      if (!intrinsic) {
        a_type_ptr rt = qbe_underlying(a->type);
        if (rt->kind == tk_pointer) rt = qbe_underlying(rt->variant.pointer.type);
        result = qbe_emit_call(b, a->kind == enk_routine ? a->variant.routine.ptr : NULL, rt, target.operand, argc, args, TRUE);
      }
      qbe_free(b, nodes); qbe_free(b, args);
      return qbe_cast(b, result, n->type);
    }
    case eok_va_start: case eok_va_start_single_operand:
      qbe_va_start(b, a->is_lvalue ? qbe_address(b, a) : qbe_expression(b, a).operand); return qbe_value(n->type, "");
    case eok_va_end: qbe_expression(b, a); return qbe_value(n->type, "");
    case eok_va_copy: {
      const char *dst = a->is_lvalue ? qbe_address(b, a) : qbe_expression(b, a).operand;
      const char *src = c->is_lvalue ? qbe_address(b, c) : qbe_expression(b, c).operand;
      qbe_copy(b, dst, src, 32); return qbe_value(n->type, "");
    }
    case eok_va_arg: {
      const char *p = a->is_lvalue ? qbe_address(b, a) : qbe_expression(b, a).operand;
      return qbe_va_arg(b, n->type, p);
    }
    default: break;
  }
  a_boolean assignment = op == eok_assign || op == eok_bassign;
  an_expr_operator_kind basic = op;
  switch (op) {
    case eok_add_assign: basic = eok_add; assignment = TRUE; break;
    case eok_subtract_assign: basic = eok_subtract; assignment = TRUE; break;
    case eok_multiply_assign: basic = eok_multiply; assignment = TRUE; break;
    case eok_divide_assign: basic = eok_divide; assignment = TRUE; break;
    case eok_remainder_assign: basic = eok_remainder; assignment = TRUE; break;
    case eok_shiftl_assign: basic = eok_shiftl; assignment = TRUE; break;
    case eok_shiftr_assign: basic = eok_shiftr; assignment = TRUE; break;
    case eok_and_assign: basic = eok_and; assignment = TRUE; break;
    case eok_or_assign: basic = eok_or; assignment = TRUE; break;
    case eok_xor_assign: basic = eok_xor; assignment = TRUE; break;
    case eok_padd_assign: basic = eok_padd; assignment = TRUE; break;
    case eok_psubtract_assign: basic = eok_psubtract; assignment = TRUE; break;
    default: break;
  }
  a_boolean increment = op == eok_pre_incr || op == eok_post_incr || op == eok_pre_decr || op == eok_post_decr;
  QbeValue x = qbe_void_value(), y = qbe_void_value();
  const char *dest = ""; a_field_ptr f = selected_field(a);
  if (assignment || increment) {
    if (c && n->variant.operation.eval_right_to_left) y = qbe_expression(b, c);
    dest = qbe_address(b, a);
    if (op != eok_assign && op != eok_bassign) x = f && f->is_bit_field ? read_bits(b, a->type, dest, f) : qbe_load(b, a->type, dest);
    if ((op == eok_post_incr || op == eok_post_decr) && wide(a->type)) {
      const char *saved = qbe_allocate(b, qbe_size(a->type), qbe_alignment(a->type)); qbe_copy(b, saved, x.operand, qbe_size(a->type)); x.operand = saved;
    }
    if (c && !n->variant.operation.eval_right_to_left) y = qbe_expression(b, c);
    if (increment) {
      basic = qbe_underlying(a->type)->kind == tk_pointer ? eok_padd : eok_add;
      a_boolean down = op == eok_pre_decr || op == eok_post_decr;
      if (basic == eok_padd) y = qbe_value(qbe_integer_type(b, 64, TRUE), down ? "-1" : "1");
      else y = qbe_cast(b, qbe_value(qbe_integer_type(b, 32, TRUE), down ? "-1" : "1"), a->type);
    }
  } else if (n->variant.operation.eval_right_to_left) { y = qbe_expression(b, c); x = qbe_expression(b, a); }
  else { x = qbe_expression(b, a); if (c) y = qbe_expression(b, c); }
  QbeValue result;
  if (op == eok_bassign) { qbe_copy(b, dest, y.operand, qbe_size(c->type)); return qbe_value(n->type, ""); }
  if (op == eok_assign) result = qbe_cast(b, y, a->type);
  else if (basic == eok_padd || basic == eok_psubtract || basic == eok_pdiff) {
    if (n->variant.operation.pointer_operand_is_second && !assignment) { QbeValue tmp = x; x = y; y = tmp; }
    const char *stride = qbe_runtime_size(b, qbe_underlying(x.type)->variant.pointer.type);
    if (EQ(stride, "0")) stride = "1";
    const char *rhs;
    if (basic == eok_pdiff) rhs = y.operand;
    else {
      y = qbe_cast(b, y, qbe_integer_type(b, 64, qbe_signed_integer(y.type))); rhs = y.operand;
      if (!EQ(stride, "1")) rhs = E("l", "mul %s, %s", rhs, stride);
    }
    const char *r = E("l", "%s %s, %s", basic == eok_padd ? "add" : "sub", x.operand, rhs);
    if (basic == eok_pdiff && !EQ(stride, "1")) r = E("l", "div %s, %s", r, stride);
    result = qbe_value(basic == eok_pdiff ? n->type : x.type, r);
  } else result = binary(b, basic, x, y, assignment || increment ? a->type : n->type);
  if (assignment || increment) {
    result = qbe_cast(b, result, a->type);
    if (f && f->is_bit_field) result = write_bits(b, a->type, result, dest, f);
    else qbe_store(b, a->type, result, dest);
    if (op == eok_post_incr || op == eok_post_decr) return qbe_cast(b, x, n->type);
  }
  return qbe_cast(b, result, n->type);
}

static void initialize_constant(QbeBackend *b, a_constant_ptr c, a_type_ptr ty, const char *where) {
  if (c->kind == ck_dynamic_init) { qbe_initialize(b, c->variant.dynamic_init.ptr, where, ty); return; }
  if (c->kind != ck_aggregate) { qbe_store(b, ty, qbe_constant(b, c), where); return; }
  a_type_ptr u = qbe_underlying(ty);
  qbe_zero(b, where, qbe_size(ty));
  uint64_t array_offset = 0;
  a_boolean sequential = u->kind == tk_array || u->kind == tk_vector;
  a_type_ptr sequence_element = u->kind == tk_array ? u->variant.array.element_type : u->kind == tk_vector ? u->variant.vector.element_type : NULL;
  /* Lowering removes constants for optimized empty fields.  Match its field
     traversal so the next real initializer cannot target overlapping storage. */
  a_field_ptr field = sequential ? NULL : next_non_empty_initializable_field(u->variant.class_struct_union.field_list);
  for (a_constant_ptr e = c->variant.aggregate.first_constant; e; e = e->next) {
    if (e->kind == ck_designator) {
      if (e->variant.designator.is_generic) qbe_fail("generic designator survived lowering");
      if (e->variant.designator.is_field_designator) field = next_non_empty_initializable_field(e->variant.designator.variant.field);
      else array_offset = e->variant.designator.variant.array_element * qbe_size(sequence_element);
      continue;
    }
    uint64_t count = e->kind == ck_init_repeat ? e->variant.init_repeat.count : 1;
    a_constant_ptr value = e->kind == ck_init_repeat ? e->variant.init_repeat.constant : e;
    if (!count) qbe_fail("runtime-sized initialization repetition survived lowering");
    for (uint64_t i = 0; i < count; ++i) {
      if (sequential) {
        a_type_ptr element = sequence_element;
        if (e->kind == ck_init_repeat && value->type && qbe_size(value->type) < qbe_size(element)) element = value->type;
        initialize_constant(b, value, element, qbe_offset(b, where, array_offset));
        array_offset += qbe_size(element);
      } else {
        if (!field) qbe_fail("aggregate initializer exceeds field list");
        const char *loc = qbe_offset(b, where, field->offset);
        if (field->is_bit_field) {
          if (value->kind == ck_dynamic_init) {
            a_dynamic_init_ptr d = value->variant.dynamic_init.ptr;
            if (d->kind != dik_expression) qbe_fail("non-expression bit-field initializer");
            write_bits(b, field->type, qbe_expression(b, d->variant.expression), loc, field);
          } else write_bits(b, field->type, qbe_constant(b, value), loc, field);
        } else initialize_constant(b, value, field->type, loc);
        field = next_non_empty_initializable_field(field->next);
        if (u->kind == tk_union) break;
      }
    }
  }
}
void qbe_initialize(QbeBackend *b, a_dynamic_init_ptr di, const char *destination, a_type_ptr context_type) {
  if (!di) return;
  a_type_ptr t = di->variable ? di->variable->type : context_type;
  if (!t) {
    switch (di->kind) {
      case dik_constant: case dik_nonconstant_aggregate: case dik_lambda: t = di->variant.constant.ptr->type; break;
      case dik_expression: case dik_class_result_via_ctor: t = di->variant.expression->type; break;
      case dik_bitwise_copy: if (di->variant.bitwise_copy.source) t = di->variant.bitwise_copy.source->type; break;
      default: break;
    }
  }
  const char *p = destination;
  if (!p[0] && di->variable) p = qbe_var_address(b, di->variable);
  if (!p[0] && t) p = qbe_allocate(b, qbe_size(t), qbe_alignment(t));
  switch (di->kind) {
    case dik_none: break;
    case dik_zero: if (!t) qbe_fail("zero initializer lacks object type"); qbe_zero(b, p, qbe_size(t)); break;
    case dik_constant: case dik_nonconstant_aggregate: case dik_lambda:
      if (!t) qbe_fail("constant initializer lacks object type");
      initialize_constant(b, di->variant.constant.ptr, t, p); break;
    case dik_expression: case dik_class_result_via_ctor: {
      QbeValue v = qbe_expression(b, di->variant.expression);
      if (!di->class_rvalue_initialized_through_master_entry && qbe_underlying(v.type)->kind != tk_void) {
        if (!t) t = v.type;
        qbe_store(b, t, v, p);
      }
      break;
    }
    case dik_constructor: {
      if (di->variant.constructor.is_copy_constructor_with_implied_source || di->variant.constructor.is_array_copy)
        qbe_fail("unlowered implied-source constructor initializer");
      size_t count = 0;
      for (an_expr_node_ptr e = di->variant.constructor.args; e; e = e->next) ++count;
      an_expr_node_ptr *nodes = count && !di->variant.constructor.has_sequenced_arguments ? qbe_alloc(b, count * sizeof(*nodes)) : NULL;
      QbeValue *args = qbe_alloc(b, (count + 1) * sizeof(*args));
      args[0] = qbe_value(qbe_pointer_type(b), p);
      size_t i = 0;
      if (di->variant.constructor.has_sequenced_arguments) {
        for (an_expr_node_ptr e = di->variant.constructor.args; e; e = e->next) args[++i] = qbe_expression(b, e);
      } else {
        for (an_expr_node_ptr e = di->variant.constructor.args; e; e = e->next) nodes[i++] = e;
        for (i = count; i; --i) args[i] = qbe_expression(b, nodes[i - 1]);
      }
      a_routine_ptr r = di->variant.constructor.ptr;
      qbe_emit_call(b, r, r->type, F("$%s", qbe_routine_symbol(b, r)), count + 1, args, TRUE);
      qbe_free(b, nodes); qbe_free(b, args); break;
    }
    case dik_bitwise_copy:
      if (!di->variant.bitwise_copy.source || !t) qbe_fail("implied-source bitwise initializer survived lowering");
      qbe_copy(b, p, qbe_address(b, di->variant.bitwise_copy.source), qbe_size(t)); break;
    default: qbe_fail(F("unsupported dynamic initializer %d", (int)di->kind));
  }
  qbe_initialized(b, di);
}

static const char *atomic_order(QbeBackend *b, const QbeValue *args, size_t i) {
  return qbe_cast(b, args[i], qbe_integer_type(b, 32, FALSE)).operand;
}
static QbeValue wide_atomic(QbeBackend *b, const char *op, a_type_ptr type, const QbeValue *args) {
  if (EQ(op, "load")) {
    const char *out = qbe_allocate(b, 16, 16);
    RAW("__atomic_load", "", {"l", "16"}, {"l", args[0].operand}, {"l", out}, {"w", atomic_order(b, args, 1)});
    return qbe_value(type, out);
  }
  if (EQ(op, "store")) {
    RAW("__atomic_store", "", {"l", "16"}, {"l", args[0].operand}, {"l", args[1].operand}, {"w", atomic_order(b, args, 2)});
    return qbe_value(type, "");
  }
  if (EQ(op, "exchange")) {
    const char *out = qbe_allocate(b, 16, 16);
    RAW("__atomic_exchange", "", {"l", "16"}, {"l", args[0].operand}, {"l", args[1].operand}, {"l", out}, {"w", atomic_order(b, args, 2)});
    return qbe_value(type, out);
  }
  if (EQ(op, "compare_exchange"))
    return qbe_value(type, RAW("__atomic_compare_exchange", "w", {"l", "16"}, {"l", args[0].operand}, {"l", args[1].operand}, {"l", args[2].operand}, {"w", atomic_order(b, args, 4)}, {"w", atomic_order(b, args, 5)}));
  a_boolean fetch_first = strncmp(op, "fetch_", 6) == 0;
  const char *arithmetic = fetch_first ? op + 6 : op;
  size_t len = strlen(arithmetic);
  if (!fetch_first && len > 6 && EQ(arithmetic + len - 6, "_fetch")) arithmetic = qbe_string(b, arithmetic, len - 6);
  an_expr_operator_kind operation;
  if (EQ(arithmetic, "add")) operation = eok_add;
  else if (EQ(arithmetic, "sub")) operation = eok_subtract;
  else if (EQ(arithmetic, "and") || EQ(arithmetic, "nand")) operation = eok_and;
  else if (EQ(arithmetic, "or")) operation = eok_or;
  else if (EQ(arithmetic, "xor")) operation = eok_xor;
  else qbe_fail(F("unsupported wide atomic operation %s", op));
  /* Concrete EDG signatures use void*: the value carries the integer type. */
  a_type_ptr element = args[1].type;
  const char *out = qbe_allocate(b, 16, 16);
  RAW("__atomic_load", "", {"l", "16"}, {"l", args[0].operand}, {"l", out}, {"w", "5"});
  const char *retry = qbe_new_label(b, "b"), *done = qbe_new_label(b, "b"), *desired = qbe_allocate(b, 16, 16);
  qbe_jump(b, retry); qbe_label(b, retry);
  QbeValue candidate = binary(b, operation, qbe_value(element, out), args[1], element);
  if (EQ(arithmetic, "nand")) {
    const char *ones = qbe_allocate(b, 16, 16); L("storel -1, %s", ones); L("storel -1, %s", qbe_offset(b, ones, 8));
    candidate = binary(b, eok_xor, candidate, qbe_value(element, ones), element);
  }
  qbe_copy(b, desired, candidate.operand, 16);
  const char *success = RAW("__atomic_compare_exchange", "w", {"l", "16"}, {"l", args[0].operand}, {"l", out}, {"l", desired}, {"w", atomic_order(b, args, 2)}, {"w", "0"});
  qbe_branch(b, success, done, retry); qbe_label(b, done);
  return qbe_value(type, fetch_first ? out : desired);
}

static void require_args(QbeBackend *b, const char *name, size_t argc, size_t n) {
  if (argc < n) qbe_fail(F("missing arguments for %s", name));
}
static const char *count64(QbeBackend *b, const char *helper, const char *value) {
  return qbe_cast(b, qbe_value(qbe_integer_type(b, 64, FALSE), RAW(F("__edg_qbe_%s64", helper), "l", {"l", value})), qbe_integer_type(b, 32, TRUE)).operand;
}
static const char *narrow_bits(QbeBackend *b, const char *value, unsigned precision) {
  return precision == 64 ? value : E("l", "and %s, %llu", value, (unsigned long long)((UINT64_C(1) << precision) - 1));
}
typedef struct {
  const char *helper, *low, *high;
  unsigned bits;
  a_boolean leading, population;
} BitCountContext;
static const char *bit_count(QbeBackend *b, const BitCountContext *c) {
  if (c->bits <= 64) {
    const char *value = count64(b, c->helper, c->low);
    return c->leading && c->bits < 64 ? E("w", "sub %s, %u", value, 64 - c->bits) : value;
  }
  if (c->population) {
    const char *low_count = count64(b, c->helper, c->low), *high_count = count64(b, c->helper, c->high);
    return E("w", "add %s, %s", low_count, high_count);
  }
  const char *first = c->leading ? c->high : c->low, *second = c->leading ? c->low : c->high;
  const char *nonzero = qbe_new_label(b, "b"), *zero = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b");
  qbe_branch(b, E("w", "cnel %s, 0", first), nonzero, zero);
  qbe_label(b, nonzero);
  const char *first_count = count64(b, c->helper, first);
  if (c->leading && c->bits < 128) first_count = E("w", "sub %s, %u", first_count, 128 - c->bits);
  qbe_jump(b, end); qbe_label(b, zero);
  const char *second_count = E("w", "add %s, %u", count64(b, c->helper, second), c->leading ? c->bits - 64 : 64);
  qbe_jump(b, end); qbe_label(b, end);
  return E("w", "phi %s %s, %s %s", nonzero, first_count, zero, second_count);
}
static a_boolean integer_intrinsic(QbeBackend *b, const char *name, const char *plain, const char *prefix, const char *helper, a_type_ptr rt, size_t argc, const QbeValue *args, QbeValue *result) {
  size_t plen = strlen(prefix);
  if (strncmp(plain, prefix, plen) != 0) return FALSE;
  const char *suffix = plain + plen;
  a_boolean generic = EQ(suffix, "g");
  if (!generic && suffix[0] && !EQ(suffix, "l") && !EQ(suffix, "ll")) return FALSE;
  require_args(b, name, argc, 1);
  unsigned bits = qbe_width(args[0].type);
  if (!generic) {
    *result = qbe_value(rt, RAW(F("__edg_qbe_%s%u", helper, bits), "w", {"l", qbe_cast(b, args[0], qbe_integer_type(b, 64, FALSE)).operand}));
    return TRUE;
  }
  if (!bits || bits > 128) qbe_fail("generic bit count exceeds native integer precision");
  BitCountContext c = {helper, NULL, NULL, bits, EQ(prefix, "clz"), EQ(prefix, "popcount")};
  if (bits <= 64) c.low = narrow_bits(b, qbe_cast(b, args[0], qbe_integer_type(b, 64, FALSE)).operand, bits);
  else {
    c.low = qbe_load(b, qbe_integer_type(b, 64, FALSE), args[0].operand).operand;
    c.high = narrow_bits(b, qbe_load(b, qbe_integer_type(b, 64, FALSE), qbe_offset(b, args[0].operand, 8)).operand, bits - 64);
  }
  const char *value;
  if (!c.population && argc > 1) {
    const char *nonzero = qbe_new_label(b, "b"), *zero = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b");
    const char *input = bits <= 64 ? c.low : E("l", "or %s, %s", c.low, c.high);
    qbe_branch(b, E("w", "cnel %s, 0", input), nonzero, zero);
    qbe_label(b, nonzero);
    const char *nonzero_count = bit_count(b, &c), *nonzero_end = b->currentLabel_;
    qbe_jump(b, end); qbe_label(b, zero);
    const char *zero_count = qbe_cast(b, args[1], qbe_integer_type(b, 32, TRUE)).operand;
    qbe_jump(b, end); qbe_label(b, end);
    value = E("w", "phi %s %s, %s %s", nonzero_end, nonzero_count, zero, zero_count);
  } else value = bit_count(b, &c);
  *result = qbe_cast(b, qbe_value(qbe_integer_type(b, 32, TRUE), value), rt);
  return TRUE;
}
static a_boolean string_in(const char *s, const char *const *set, size_t count) {
  for (size_t i = 0; i < count; ++i) if (EQ(s, set[i])) return TRUE;
  return FALSE;
}
#define IN(s, set) string_in((s), (set), sizeof(set) / sizeof((set)[0]))
#define BUILTIN_RAW(symbol, return_type, ...) (*result = qbe_value(rt, RAW((symbol), (return_type), __VA_ARGS__)))

a_boolean qbe_builtin(QbeBackend *b, a_routine_ptr routine, size_t argc, const QbeValue *args, QbeValue *result) {
  if (!routine->source_corresp.name) return FALSE;
  const char *name = routine->source_corresp.name;
  a_type_ptr rt = qbe_underlying(routine->type)->variant.routine.return_type;
  if (EQ(name, "__builtin_expect") || EQ(name, "__builtin_expect_with_probability") || EQ(name, "__builtin_launder") || EQ(name, "__builtin_assume_aligned")) {
    require_args(b, name, argc, 1); *result = qbe_cast(b, args[0], rt); return TRUE;
  }
  if (EQ(name, "__builtin_is_constant_evaluated") || EQ(name, "__builtin_constant_p")) { *result = qbe_value(rt, "0"); return TRUE; }
  if (EQ(name, "__builtin_unreachable")) { qbe_line(b, "hlt"); b->terminated_ = TRUE; *result = qbe_value(rt, ""); return TRUE; }
  if (EQ(name, "__builtin_trap")) { qbe_runtime_raw(b, "abort", "", 0, NULL); qbe_line(b, "hlt"); b->terminated_ = TRUE; *result = qbe_value(rt, ""); return TRUE; }
  if (EQ(name, "__builtin_prefetch")) { *result = qbe_value(rt, ""); return TRUE; }
  if (EQ(name, "__builtin_object_size") || EQ(name, "__builtin_dynamic_object_size")) {
    require_args(b, name, argc, 2);
    const char *mode = qbe_cast(b, args[1], qbe_integer_type(b, 32, FALSE)).operand;
    const char *bit = E("w", "and %s, 2", mode), *cond;
    cond = E("w", "cnew %s, 0", bit);
    const char *yes = qbe_new_label(b, "b"), *no = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b"), *slot = qbe_allocate(b, 8, 8);
    qbe_branch(b, cond, yes, no); qbe_label(b, yes); L("storel 0, %s", slot); qbe_jump(b, end);
    qbe_label(b, no); L("storel -1, %s", slot); qbe_jump(b, end); qbe_label(b, end);
    *result = qbe_cast(b, qbe_value(qbe_integer_type(b, 64, FALSE), E("l", "loadl %s", slot)), rt); return TRUE;
  }
  if (strncmp(name, "__builtin_", 10) == 0) {
    const char *plain = name + 10, *overflow_name = plain;
    size_t len = strlen(overflow_name);
    a_boolean overflow_predicate = len > 2 && EQ(overflow_name + len - 2, "_p");
    if (overflow_predicate) overflow_name = qbe_string(b, overflow_name, len - 2);
    unsigned overflow_op = 3;
    static const char *const adds[] = {"add_overflow", "sadd_overflow", "uadd_overflow", "saddl_overflow", "uaddl_overflow", "saddll_overflow", "uaddll_overflow"};
    static const char *const subs[] = {"sub_overflow", "ssub_overflow", "usub_overflow", "ssubl_overflow", "usubl_overflow", "ssubll_overflow", "usubll_overflow"};
    static const char *const muls[] = {"mul_overflow", "smul_overflow", "umul_overflow", "smull_overflow", "umull_overflow", "smulll_overflow", "umulll_overflow"};
    if (IN(overflow_name, adds)) overflow_op = 0;
    if (IN(overflow_name, subs)) overflow_op = 1;
    if (IN(overflow_name, muls)) overflow_op = 2;
    if (overflow_op != 3) {
      require_args(b, name, argc, 3);
      a_type_ptr out_type = overflow_predicate ? args[2].type : qbe_underlying(args[2].type)->variant.pointer.type;
      const char *out = overflow_predicate ? qbe_allocate(b, qbe_size(out_type), qbe_alignment(out_type)) : args[2].operand;
      const char *lhs = qbe_allocate(b, qbe_size(args[0].type), qbe_alignment(args[0].type));
      const char *rhs = qbe_allocate(b, qbe_size(args[1].type), qbe_alignment(args[1].type));
      qbe_store(b, args[0].type, args[0], lhs); qbe_store(b, args[1].type, args[1], rhs);
      BUILTIN_RAW("__edg_qbe_overflow", "w",
        {"l", out}, {"w", F("%u", qbe_width(out_type))}, {"w", qbe_signed_integer(out_type) ? "0" : "1"},
        {"l", lhs}, {"w", F("%u", qbe_width(args[0].type))}, {"w", qbe_signed_integer(args[0].type) ? "0" : "1"},
        {"l", rhs}, {"w", F("%u", qbe_width(args[1].type))}, {"w", qbe_signed_integer(args[1].type) ? "0" : "1"},
        {"w", F("%u", overflow_op)});
      return TRUE;
    }
    if (EQ(plain, "isnan") || EQ(plain, "isinf") || EQ(plain, "isinf_sign") || EQ(plain, "isfinite") || EQ(plain, "isnormal") || EQ(plain, "signbit") || EQ(plain, "signbitf") || EQ(plain, "signbitl")) {
      require_args(b, name, argc, 1); QbeValue v = args[0];
      if (wide(v.type)) {
        BUILTIN_RAW(F("__edg_qbe_tf_%s", strncmp(plain, "signbit", 7) == 0 ? "signbit" : plain), "w", {"l", v.operand}); return TRUE;
      }
      const char *t = qbe_base_type(b, v.type), *it = EQ(t, "s") ? "w" : "l";
      unsigned bits = EQ(t, "s") ? 32 : 64;
      const char *repr = E(it, "cast %s", v.operand);
      const char *max = bits == 32 ? "2139095040" : "9218868437227405312";
      const char *exponent = E(it, "and %s, %s", repr, max);
      const char *inf = E("w", "ceq%s %s, %s", it, exponent, max);
      if (EQ(plain, "isnan")) *result = qbe_value(rt, E("w", "cuo%s %s, %s", t, v.operand, v.operand));
      else if (EQ(plain, "isfinite")) *result = qbe_value(rt, E("w", "ceqw %s, 0", inf));
      else if (EQ(plain, "isnormal")) {
        const char *nonzero = E("w", "cne%s %s, 0", it, exponent);
        *result = qbe_value(rt, E("w", "and %s, %s", nonzero, E("w", "ceqw %s, 0", inf)));
      } else if (strncmp(plain, "signbit", 7) == 0)
        *result = qbe_cast(b, qbe_value(qbe_integer_type(b, bits, FALSE), E(it, "shr %s, %u", repr, bits - 1)), rt);
      else {
        const char *mantissa = E(it, "and %s, %s", repr, bits == 32 ? "8388607" : "4503599627370495");
        const char *zero_mantissa = E("w", "ceq%s %s, 0", it, mantissa);
        const char *isinf = E("w", "and %s, %s", inf, zero_mantissa);
        if (EQ(plain, "isinf_sign")) {
          const char *sign = E(it, "sar %s, %u", repr, bits - 1);
          const char *signed_one = E("w", "or %s, 1", sign);
          isinf = E("w", "mul %s, %s", isinf, signed_one);
        }
        *result = qbe_value(rt, isinf);
      }
      return TRUE;
    }
    if (EQ(plain, "isgreater") || EQ(plain, "isgreaterequal") || EQ(plain, "isless") || EQ(plain, "islessequal") || EQ(plain, "islessgreater") || EQ(plain, "isunordered")) {
      require_args(b, name, argc, 2);
      if (EQ(plain, "isunordered")) {
        if (wide(args[0].type)) BUILTIN_RAW("__edg_qbe_tf_isunordered", "w", {"l", args[0].operand}, {"l", args[1].operand});
        else *result = qbe_value(rt, E("w", "cuo%s %s, %s", qbe_base_type(b, args[0].type), args[0].operand, args[1].operand));
      } else if (EQ(plain, "islessgreater")) {
        QbeValue lt = binary(b, eok_lt, args[0], args[1], rt), gt = binary(b, eok_gt, args[0], args[1], rt);
        *result = qbe_value(rt, E("w", "or %s, %s", lt.operand, gt.operand));
      } else *result = binary(b, EQ(plain, "isgreater") ? eok_gt : EQ(plain, "isgreaterequal") ? eok_ge : EQ(plain, "isless") ? eok_lt : eok_le, args[0], args[1], rt);
      return TRUE;
    }
    static const char *const memory_functions[] = {"memcpy", "memmove", "memset", "memcmp", "strlen", "strcmp", "strcpy", "strncpy", "strchr"};
    if (IN(plain, memory_functions)) { *result = qbe_runtime(b, plain, rt, argc, args, FALSE); return TRUE; }
    if (EQ(plain, "alloca") || EQ(plain, "alloca_with_align")) {
      require_args(b, name, argc, 1); unsigned align = 16;
      if (EQ(plain, "alloca_with_align")) {
        require_args(b, name, argc, 2);
        if (!EQ(args[1].operand, "128") && !EQ(args[1].operand, "64") && !EQ(args[1].operand, "32")) qbe_fail("unsupported dynamic alloca alignment");
      }
      *result = qbe_value(rt, E("l", "alloc%u %s", align, qbe_cast(b, args[0], qbe_integer_type(b, 64, FALSE)).operand));
      qbe_keep_stack(b);
      return TRUE;
    }
    if (integer_intrinsic(b, name, plain, "popcount", "ctpop", rt, argc, args, result) ||
        integer_intrinsic(b, name, plain, "clz", "ctlz", rt, argc, args, result) ||
        integer_intrinsic(b, name, plain, "ctz", "cttz", rt, argc, args, result)) return TRUE;
    if (EQ(plain, "bswap16") || EQ(plain, "bswap32") || EQ(plain, "bswap64")) {
      require_args(b, name, argc, 1);
      BUILTIN_RAW(F("__edg_qbe_%s", plain), qbe_base_type(b, rt), {"l", qbe_cast(b, args[0], qbe_integer_type(b, 64, FALSE)).operand}); return TRUE;
    }
    if (EQ(plain, "ffs") || EQ(plain, "ffsl") || EQ(plain, "ffsll") || EQ(plain, "parity") || EQ(plain, "parityl") || EQ(plain, "parityll")) {
      require_args(b, name, argc, 1); unsigned bits = qbe_width(args[0].type);
      a_boolean parity = strncmp(plain, "parity", 6) == 0;
      const char *value = RAW(F("__edg_qbe_%s%u", parity ? "ctpop" : "cttz", bits), "w", {"l", qbe_cast(b, args[0], qbe_integer_type(b, 64, FALSE)).operand});
      if (parity) *result = qbe_value(rt, E("w", "and %s, 1", value));
      else {
        const char *slot = qbe_allocate(b, 4, 8), *yes = qbe_new_label(b, "b"), *no = qbe_new_label(b, "b"), *end = qbe_new_label(b, "b");
        qbe_branch(b, qbe_truth(b, args[0]), yes, no); qbe_label(b, yes); L("storew %s, %s", E("w", "add %s, 1", value), slot); qbe_jump(b, end);
        qbe_label(b, no); L("storew 0, %s", slot); qbe_jump(b, end); qbe_label(b, end); *result = qbe_load(b, rt, slot);
      }
      return TRUE;
    }
    const char *math_name = plain;
    size_t math_len = strlen(plain);
    if (math_len && (plain[math_len - 1] == 'f' || plain[math_len - 1] == 'l')) math_name = qbe_string(b, plain, math_len - 1);
    static const char *const math_functions[] = {
      "sqrt", "fabs", "copysign", "sin", "cos", "tan", "asin", "acos", "atan", "atan2",
      "sinh", "cosh", "tanh", "exp", "exp2", "expm1", "log", "log2", "log10", "log1p",
      "pow", "floor", "ceil", "trunc", "round", "rint", "nearbyint", "fmod", "remainder",
      "fma", "fmin", "fmax", "fdim", "hypot", "cbrt", "ldexp", "scalbn", "frexp", "modf",
      "llround", "lround", "llrint", "lrint", "nextafter", "nexttoward", "erf", "erfc", "tgamma", "lgamma"
    };
    if (IN(plain, math_functions) || IN(math_name, math_functions)) { *result = qbe_runtime(b, plain, rt, argc, args, FALSE); return TRUE; }
    static const char *const libc_functions[] = {
      "abort", "exit", "_Exit", "malloc", "calloc", "realloc", "free", "aligned_alloc",
      "puts", "putchar", "printf", "fprintf", "sprintf", "snprintf", "vprintf", "vfprintf", "vsprintf", "vsnprintf",
      "memchr", "mempcpy", "bcmp", "strcat", "strncat", "strncmp", "strrchr", "strstr", "strspn", "strcspn",
      "__memcpy_chk", "__memmove_chk", "__memset_chk", "__strcpy_chk", "__strncpy_chk", "__strcat_chk",
      "__sprintf_chk", "__snprintf_chk", "__vsprintf_chk", "__vsnprintf_chk"
    };
    if (IN(plain, libc_functions)) { *result = qbe_runtime(b, plain, rt, argc, args, FALSE); return TRUE; }
  }
  if (EQ(name, "__atomic_thread_fence") || EQ(name, "__atomic_signal_fence") || EQ(name, "__sync_synchronize")) {
    RAW("__edg_qbe_fence", "", {"w", argc == 0 ? "5" : qbe_cast(b, args[0], qbe_integer_type(b, 32, FALSE)).operand});
    *result = qbe_value(rt, ""); return TRUE;
  }
  if (EQ(name, "__atomic_always_lock_free") || EQ(name, "__atomic_is_lock_free")) {
    require_args(b, name, argc, 2); BUILTIN_RAW("__atomic_is_lock_free", "w", {"l", qbe_cast(b, args[0], qbe_integer_type(b, 64, FALSE)).operand}, {"l", args[1].operand}); return TRUE;
  }
  if (strncmp(name, "__atomic_", 9) == 0) {
    require_args(b, name, argc, 1);
    const char *operation = name + 9;
    uint64_t bytes = 0;
    const char *suffix = strrchr(operation, '_');
    if (suffix) {
      const char *width = suffix + 1;
      if (EQ(width, "1")) bytes = 1;
      else if (EQ(width, "2")) bytes = 2;
      else if (EQ(width, "4")) bytes = 4;
      else if (EQ(width, "8")) bytes = 8;
      else if (EQ(width, "16")) bytes = 16;
      if (bytes) operation = qbe_string(b, operation, (size_t)(suffix - operation));
    }
    a_boolean nongeneric = bytes != 0;
    if (EQ(operation, "load_n") || EQ(operation, "store_n") || EQ(operation, "exchange_n") || EQ(operation, "compare_exchange_n")) {
      operation = qbe_string(b, operation, strlen(operation) - 2); nongeneric = TRUE;
    }
    a_boolean transfer = EQ(operation, "load") || EQ(operation, "store") || EQ(operation, "exchange") || EQ(operation, "compare_exchange");
    if (transfer && !nongeneric) {
      /* EDG supplies size_t then storage pointers; libatomic is always strong. */
      size_t count = EQ(operation, "compare_exchange") ? 7 : EQ(operation, "exchange") ? 5 : 4;
      if (argc != count || qbe_underlying(args[0].type)->kind != tk_integer)
        qbe_fail(F("invalid generic atomic argument representation for %s: expected size_t followed by %zu arguments", name, count - 1));
      size_t last_pointer = EQ(operation, "exchange") || EQ(operation, "compare_exchange") ? 3 : 2;
      QbeRawArg rawargs[7]; size_t rawc = 0;
      rawargs[rawc++] = (QbeRawArg){"l", qbe_cast(b, args[0], qbe_integer_type(b, 64, FALSE)).operand};
      for (size_t i = 1; i < argc; ++i) {
        if (EQ(operation, "compare_exchange") && i == 4) continue;
        a_boolean ptr = i <= last_pointer;
        if (ptr && qbe_underlying(args[i].type)->kind != tk_pointer)
          qbe_fail(F("expected storage pointer at argument %zu of %s", i + 1, name));
        a_type_ptr ty = ptr ? qbe_pointer_type(b) : qbe_integer_type(b, 32, FALSE);
        rawargs[rawc++] = (QbeRawArg){qbe_base_type(b, ty), qbe_cast(b, args[i], ty).operand};
      }
      *result = qbe_value(rt, qbe_runtime_raw(b, F("__atomic_%s", operation), EQ(operation, "compare_exchange") ? "w" : "", rawc, rawargs)); return TRUE;
    }
    static const char *const arithmetic[] = {"fetch_add", "fetch_sub", "fetch_and", "fetch_or", "fetch_xor", "fetch_nand", "add_fetch", "sub_fetch", "and_fetch", "or_fetch", "xor_fetch", "nand_fetch"};
    a_boolean flag = EQ(operation, "test_and_set") || EQ(operation, "clear");
    if (!transfer && !flag && !IN(operation, arithmetic)) qbe_fail(F("unsupported GNU atomic intrinsic %s", name));
    size_t count = EQ(operation, "compare_exchange") ? 6 : EQ(operation, "load") || flag ? 2 : 3;
    if (argc != count) qbe_fail(F("invalid scalar atomic argument count for %s: expected %zu", name, count));
    a_type_ptr pt = qbe_underlying(args[0].type);
    if (pt->kind != tk_pointer) qbe_fail(F("expected atomic storage pointer for %s", name));
    if (EQ(operation, "compare_exchange") && qbe_underlying(args[1].type)->kind != tk_pointer) qbe_fail(F("expected comparison storage pointer for %s", name));
    if (flag) bytes = 1;
    else if (!bytes) bytes = qbe_size(pt->variant.pointer.type);
    if (bytes != 1 && bytes != 2 && bytes != 4 && bytes != 8 && bytes != 16)
      qbe_fail(F("unsupported scalar atomic storage width %llu for %s", (unsigned long long)bytes, name));
    QbeValue values[6];
    size_t valuec = argc;
    for (size_t i = 0; i < argc; ++i) values[i] = args[i];
    if (flag) {
      values[2] = values[1];
      values[1] = qbe_value(qbe_integer_type(b, 8, FALSE), EQ(operation, "test_and_set") ? "1" : "0");
      operation = EQ(operation, "test_and_set") ? "exchange" : "store";
      ++valuec;
    }
    if (bytes == 16) {
      if (!EQ(operation, "load")) {
        size_t data = EQ(operation, "compare_exchange") ? 2 : 1;
        values[data] = qbe_cast(b, values[data], qbe_integer_type(b, 128, FALSE));
      }
      *result = wide_atomic(b, operation, rt, values); return TRUE;
    }
    QbeRawArg rawargs[6]; size_t rawc = 0;
    for (size_t i = 0; i < valuec; ++i) {
      if (EQ(operation, "compare_exchange") && i == 3) continue;
      a_boolean ptr = i == 0 || (EQ(operation, "compare_exchange") && i == 1);
      a_boolean data = !ptr && i == 1 && !EQ(operation, "load");
      if (EQ(operation, "compare_exchange") && i == 2) data = TRUE;
      a_type_ptr ty = ptr ? qbe_pointer_type(b) : data ? qbe_integer_type(b, bytes * 8, FALSE) : qbe_integer_type(b, 32, FALSE);
      QbeValue value = qbe_cast(b, values[i], ty);
      rawargs[rawc++] = (QbeRawArg){qbe_base_type(b, ty), value.operand};
    }
    *result = qbe_value(rt, qbe_runtime_raw(b, F("__atomic_%s_%llu", operation, (unsigned long long)bytes), qbe_underlying(rt)->kind == tk_void ? "" : qbe_base_type(b, rt), rawc, rawargs)); return TRUE;
  }
  if (strncmp(name, "__sync_", 7) == 0) {
    require_args(b, name, argc, 1);
    a_type_ptr element = qbe_underlying(args[0].type)->variant.pointer.type;
    uint64_t bytes = qbe_size(element);
    const char *suffix = F("_%llu", (unsigned long long)bytes), *op = name + 7;
    if (bytes == 16) {
      if (EQ(op, "bool_compare_and_swap") || EQ(op, "val_compare_and_swap")) {
        require_args(b, name, argc, 3);
        const char *expected = qbe_allocate(b, 16, 16); qbe_store(b, element, args[1], expected);
        QbeValue values[] = {args[0], qbe_value(qbe_pointer_type(b), expected), args[2], qbe_value(qbe_integer_type(b, 32, FALSE), "0"), qbe_value(qbe_integer_type(b, 32, FALSE), "5"), qbe_value(qbe_integer_type(b, 32, FALSE), "5")};
        QbeValue success = wide_atomic(b, "compare_exchange", qbe_integer_type(b, 32, FALSE), values);
        *result = EQ(op, "bool_compare_and_swap") ? qbe_cast(b, success, rt) : qbe_load(b, element, expected);
      } else if (EQ(op, "lock_release")) {
        const char *z = qbe_allocate(b, 16, 16); qbe_zero(b, z, 16);
        QbeValue values[] = {args[0], qbe_value(element, z), qbe_value(qbe_integer_type(b, 32, FALSE), "3")};
        *result = wide_atomic(b, "store", rt, values);
      } else {
        require_args(b, name, argc, 2); if (EQ(op, "lock_test_and_set")) op = "exchange";
        QbeValue values[] = {args[0], args[1], qbe_value(qbe_integer_type(b, 32, FALSE), "5")};
        *result = wide_atomic(b, op, rt, values);
      }
      return TRUE;
    }
    if (EQ(op, "bool_compare_and_swap") || EQ(op, "val_compare_and_swap")) {
      require_args(b, name, argc, 3);
      const char *expected = qbe_allocate(b, bytes, qbe_alignment(element)); qbe_store(b, element, args[1], expected);
      const char *success = RAW(F("__atomic_compare_exchange%s", suffix), "w", {"l", args[0].operand}, {"l", expected}, {qbe_base_type(b, element), qbe_cast(b, args[2], element).operand}, {"w", "5"}, {"w", "5"});
      *result = EQ(op, "bool_compare_and_swap") ? qbe_value(rt, success) : qbe_load(b, element, expected);
    } else if (EQ(op, "lock_release")) {
      RAW(F("__atomic_store%s", suffix), "", {"l", args[0].operand}, {qbe_base_type(b, element), "0"}, {"w", "3"});
      *result = qbe_value(rt, "");
    } else {
      require_args(b, name, argc, 2);
      if (EQ(op, "lock_test_and_set")) op = "exchange";
      *result = qbe_value(rt, RAW(F("__atomic_%s%s", op, suffix), qbe_base_type(b, element), {"l", args[0].operand}, {qbe_base_type(b, element), qbe_cast(b, args[1], element).operand}, {"w", "5"}));
    }
    return TRUE;
  }
  if (strncmp(name, "__builtin_", 10) == 0 || strncmp(name, "__sync_", 7) == 0) qbe_fail(F("unsupported GNU intrinsic %s", name));
  return FALSE;
}
