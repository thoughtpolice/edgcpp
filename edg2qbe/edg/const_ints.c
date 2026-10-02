/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/*

const_ints.c -- Manipulation of target integer constants.

*/

/* Header files common to all files. */
#include "fe_common.h"

#ifdef PCH_PRAGMA_GUARD
/* Mark the end of the sequence of headers subject to precompiled header
   processing. */
#pragma hdrstop
#endif /* ifdef PCH_PRAGMA_GUARD */

/* Conditionally open the "edg" namespace. */
BEGIN_EDG_NAMESPACE


#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
#define INT_VALUE_PART_BASE ((a_host_large_unsigned)MAX_UINT_VALUE_PART + 1)
#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */


#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
void set_integer_value(an_integer_value		*intval,
                       a_host_large_integer	value)
/*
Set the integer value entry *intval to the signed value "value".
*/
{
  int                  i;
  a_host_large_integer this_part;

  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    this_part = value & MAX_UINT_VALUE_PART;
    intval->part[i] = (an_int_value_part)this_part;
    value = signed_shift_right(value, BITS_IN_INT_VALUE_PART);
  }  /* if */
}  /* set_integer_value */




void set_unsigned_integer_value(an_integer_value	*intval,
                                a_host_large_unsigned	value)
/*
Set the integer value entry *intval to the unsigned value "value".
*/
{
  int                  i;
  a_host_large_integer this_part;

  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    this_part = value & MAX_UINT_VALUE_PART;
    intval->part[i] = (an_int_value_part)this_part;
    value = value >> BITS_IN_INT_VALUE_PART;
  }  /* if */
}  /* set_unsigned_integer_value */




void conv_integer_value_to_host_large_integer(
			        an_integer_value	*intval,
                                a_boolean		is_signed,
				a_host_large_integer	*value,
                                a_boolean		*err)
/*
Extract a host large integer from an_integer_value.  is_signed indicates
whether the integer value should be considered signed or unsigned.  If
is_signed is TRUE the value returned in "value" will be signed, otherwise
the value returned in "value" will be unsigned.  Set err to
TRUE if the value cannot be represented in a host large integer (or host
large unsigned if is_signed is FALSE) otherwise set err to FALSE.
*/
{
  int                   i;
  sizeof_t              bits_so_far = 0;
  sizeof_t              bits_discarded = BITS_IN_AN_INTEGER_VALUE -
                                     (sizeof(a_host_large_integer) * CHAR_BIT);
  an_int_value_part     this_part;
  an_int_value_part     empty_bits;
  a_host_large_unsigned result = 0;
  a_boolean             overflow = FALSE;
  a_boolean             is_negative;

  /* The low order parts will be used to construct the result.  The high
     order bits must be zeros if the number is positive, or ones if the
     number is negative. */
  is_negative = sign_of(*intval);
  if (is_signed && is_negative) {
    empty_bits = MAX_UINT_VALUE_PART;
  } else {
    empty_bits = 0;
  }  /* if */
  for (i = 0; i < (int)INT_VALUE_PARTS_PER_INTEGER_VALUE; ++i) {
    this_part = intval->part[i];
    if (bits_so_far < bits_discarded) {
      if (this_part != empty_bits) {
        overflow = TRUE;
      }  /* if */
    } else {
      result = (result << BITS_IN_INT_VALUE_PART) + this_part;
    }  /* if */
    bits_so_far += BITS_IN_INT_VALUE_PART;
  }  /* for */
  /* If the sign of the result is not the same as the sign of the original
     number then an overflow occurred. */
  if (is_signed && is_negative != ((a_host_large_integer)result < 0)) {
    overflow = TRUE;
  }  /* if */
  *value = (a_host_large_integer)result;
  *err = overflow;
}  /* conv_integer_value_to_host_large_integer */

#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */

void conv_integer_value_to_float(an_integer_value		*int_value,
				 a_boolean			is_signed,
			         an_internal_float_value	*float_value,
				 a_float_kind			float_kind,
				 a_boolean			*err)
/*
Convert the integer value int_value to floating-point in float_value.
is_signed is TRUE if int_value is a signed value.  Set *err to TRUE if
an error occurred during the conversion.
*/
{
  *err = FALSE;
  if (is_signed) {
    /* The source is a signed integer value. */
    a_host_large_integer    hli_temp;
    hli_temp = value_of_integer_value(int_value, /*is_signed=*/TRUE, err);
    if (!*err) {
      fp_host_large_integer_to_float(float_kind, hli_temp, float_value, err);
    }  /* if */
  } else {
    /* The source is an unsigned integer value. */
    a_host_large_unsigned   hlu_temp;
    hlu_temp = unsigned_value_of_integer_value(int_value, /*is_signed=*/FALSE,
                                               err);
    if (!*err) {
      fp_host_large_unsigned_to_float(float_kind, hlu_temp, float_value, err);
    }  /* if */
  }  /* if */
#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  if (*err) {
    /* Try again with larger precision by converting the integer to a
       character string then converting the string to a float value. */
    a_number_buffer str = str_for_integer_value(int_value, is_signed,
                                                /*non_arithmetic=*/FALSE,
                                                targ_sizeof_largest_integer);
    fp_string_to_float(float_kind, edg_string_cstr(&str), float_value, err);
    edg_string_destroy(&str);
  }  /* if */
#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* conv_integer_value_to_float */


a_boolean int_constant_is_signed(a_constant_ptr constant)
/*
Return TRUE if the given integer constant's type is signed.
*/
{
  a_type_ptr tp = constant->type;

  /* Drop typerefs, but don't use skip_typerefs, since that is not available
     in standalone utility programs. */
  while (tp->kind == (a_type_kind)tk_typeref) tp = tp->variant.typeref.type;
  return (tp->kind == (a_type_kind)tk_integer &&
          int_kind_is_signed[(int)tp->variant.integer.int_kind]);
}  /* int_constant_is_signed */


a_host_large_integer value_of_integer_value(an_integer_value	*int_value,
					    a_boolean		is_signed,
					    a_boolean		*ovflo)
/*
Retrieve the value of the int_value and return it as a host large
integer.  is_signed indicates whether int_value should be treated as signed.
If the value is not representable as a host large integer, return *ovflo TRUE.
*/
{
  a_host_large_integer	value;
  a_boolean		err;

  *ovflo = FALSE;
  conv_integer_value_to_host_large_integer(int_value, is_signed, &value, &err);
  if ((value < 0 && !is_signed) || err) {
    /* Unsigned constant with value too large to represent or an
       integer value that can't be represented as a host large integer. */
    *ovflo = TRUE;
  }  /* if */
  return value;
}  /* value_of_integer_value */


a_host_large_unsigned unsigned_value_of_integer_value(
					    an_integer_value	*int_value,
					    a_boolean		is_signed,
					    a_boolean		*ovflo)
/*
Retrieve the value of the int_value and return it as a host large
unsigned.  is_signed indicates whether int_value should be treated as signed.
If the value is not representable as a host large unsigned, return *ovflo TRUE.
*/
{
  a_host_large_integer	value;
  a_boolean		err;

  *ovflo = FALSE;
  /* Note that the value is returned in a host large integer, but the value
     is actually unsigned when is_signed is FALSE. */
  conv_integer_value_to_host_large_integer(int_value, is_signed,
                                           &value, &err);
  if (err || (is_signed && sign_of(*int_value))) {
    /* An integer value that can't be represented as a host large unsigned,
       or a value is negative. */
    *ovflo = TRUE;
  }  /* if */
  return (a_host_large_unsigned)value;
}  /* unsigned_value_of_integer_value */


a_host_large_integer value_of_integer_constant(a_constant *cp,
                                               a_boolean  *ovflo)
/*
Retrieve the value of the integer constant cp and return it as a host large
integer.  If the value is not representable as a host large integer,
return *ovflo TRUE.
*/
{
  a_host_large_integer	value;

  value = value_of_integer_value(&cp->variant.integer_value,
                                 int_constant_is_signed(cp), ovflo);
  return value;
}  /* value_of_integer_constant */


a_host_large_unsigned unsigned_value_of_integer_constant(a_constant *cp,
                                                         a_boolean  *ovflo)
/*
Retrieve the value of the integer constant cp and return it as a host
large unsigned.  If the value is not representable as a host large unsigned,
return *ovflo TRUE.
*/
{
  a_host_large_unsigned	value;

  value = unsigned_value_of_integer_value(&cp->variant.integer_value,
                                          int_constant_is_signed(cp), ovflo);
  return value;
}  /* unsigned_value_of_integer_constant */


int cmp_integer_values(an_integer_value *op_1,
		       a_boolean	op_1_signed,
		       an_integer_value *op_2,
		       a_boolean	op_2_signed)
/*
Compare the integer values op_1 and op_2, and return

  -1   if op_1 <  op_2
   0   if op_1 == op_2
  +1   if op_1 >  op_2

signed1 and signed2 give the signedness of the two values.
*/
{
#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  int			i;
#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  a_boolean		sign_1 = op_1_signed && sign_of(*op_1);
  a_boolean		sign_2 = op_2_signed && sign_of(*op_2);
  int			result = 0;

  if (sign_1 != sign_2) {
    /* If signs of the values are different then if op_1 is negative
       it must be less than op_2.  If op_1 is nonnegative it must be greater
       than op_2. */
    result = sign_1 ? -1 : 1;
  } else {
    /* The values have the same sign; just do a straight bit
       comparison until we find a difference. */
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
    if (*op_1 > *op_2) {
      result = 1;
    } else if (*op_1 < *op_2) {
      result = -1;
    } else {
      result = 0;
    }  /* if */
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
    for (i = 0; i < (int)INT_VALUE_PARTS_PER_INTEGER_VALUE; ++i) {
      if (op_1->part[i] > op_2->part[i]) {
        result = 1;
        break;
      } else if (op_1->part[i] < op_2->part[i]) {
        result = -1;
        break;
      }  /* if */
    }  /* for */
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  }  /* if */
  return result;
}  /* cmp_integer_values */


int cmp_integer_constants(a_constant *con1,
                          a_constant *con2)
/*
Compare the integer constants con1 and con2, and return

  -1   if con1 <  con2
   0   if con1 == con2
  +1   if con1 >  con2
*/
{
  int cmp;

  cmp = cmp_integer_values(&con1->variant.integer_value,
                           int_constant_is_signed(con1),
                           &con2->variant.integer_value,
                           int_constant_is_signed(con2));
  return cmp;
}  /* cmp_integer_constants */


int cmplit_integer_constant(a_constant			*con1,
                            a_host_large_integer	value2)
/*
Compare the integer constant con1 to value2, and return

  -1   if con1 <  value2
   0   if con1 == value2
  +1   if con1 >  value2
*/
{
  int              cmp;
  an_integer_value intval2;

  set_integer_value(&intval2, value2);
  cmp = cmp_integer_values(&con1->variant.integer_value,
                           int_constant_is_signed(con1),
                           &intval2,
                           TRUE);
  return cmp;
}  /* cmplit_integer_constant */


int cmpulit_integer_constant(a_constant			*con1,
                             a_host_large_unsigned	unsigned_value2)
/*
Compare the integer constant con1 to the unsigned value unsigned_value2,
and return

  -1   if con1 <  unsigned_value2
   0   if con1 == unsigned_value2
  +1   if con1 >  unsigned_value2
*/
{
  int              cmp;
  an_integer_value intval2;

  set_unsigned_integer_value(&intval2, unsigned_value2);
  cmp = cmp_integer_values(&con1->variant.integer_value,
                           int_constant_is_signed(con1),
                           &intval2,
                           FALSE);
  return cmp;
}  /* cmpulit_integer_constant */


a_boolean in_range_for_integer_kind(a_constant      *min_con,
                                    a_constant      *max_con,
                                    an_integer_kind ikind)
/*
Return TRUE if the range min_con..max_con falls entirely within the legal
range of values for integers of kind ikind.  min_con and max_con may be
the same constant.
*/
{
  a_boolean in_range = TRUE;
  a_boolean min_con_signed, max_con_signed;
  a_boolean ikind_signed = int_kind_is_signed[ikind];

  check_assertion(min_con->kind == (a_constant_repr_kind)ck_integer &&
                  max_con->kind == (a_constant_repr_kind)ck_integer);
  min_con_signed = int_constant_is_signed(min_con);
  if (cmp_integer_values(&min_con->variant.integer_value,
                         min_con_signed,
                         &min_integer_value_of_kind[ikind],
                         ikind_signed) < 0) {
    in_range = FALSE;
  } else {
    if (min_con == max_con) {
      /* If the two constants are the same constant, we don't have to
         determine the signedness again. */
      max_con_signed = min_con_signed;
    } else {
      max_con_signed = int_constant_is_signed(max_con);
    }  /* if */
    if (cmp_integer_values(&max_con->variant.integer_value,
                           max_con_signed,
                           &max_integer_value_of_kind[ikind],
                           ikind_signed) > 0) {
      in_range = FALSE;
    }  /* if */
  }  /* if */
  return in_range;
}  /* in_range_for_integer_kind */


a_targ_size_t integer_value_bit_size_for_type(a_type_ptr type)
/*
Return the number of value-representation bits used by the indicated integer
type.  For bit-precise integer types, this is the declared _BitInt width; for
ordinary integer types, this is the target storage size in bits.
*/
{
  a_targ_size_t result;

  type = skip_typerefs(type);
  check_assertion(type_is(type, tk_integer));
  if (is_bit_precise_kind(type->variant.integer.int_kind)) {
    result = bit_precise_integer_width(type);
  } else {
    result = type->size * targ_char_bit;
  }  /* if */
  return result;
}  /* integer_value_bit_size_for_type */


a_boolean integer_value_can_represent_type_width(a_type_ptr type)
/*
Return TRUE if an_integer_value can represent all value bits of the indicated
integer type.
*/
{
  return integer_value_bit_size_for_type(type) <=
                                          BITS_IN_AN_INTEGER_VALUE;
}  /* integer_value_can_represent_type_width */


void integer_value_range_for_type(a_type_ptr        type,
                                  an_integer_value  *min_value,
                                  an_integer_value  *max_value)
/*
Return in *min_value and *max_value the representable range for the indicated
integer type.
*/
{
  a_targ_size_t    bit_size;
  an_integer_kind  ikind;
  a_boolean        is_signed;

  type = skip_typerefs(type);
  check_assertion(type_is(type, tk_integer));
  ikind = type->variant.integer.int_kind;
  if (!is_bit_precise_kind(ikind)) {
    *min_value = min_integer_value_of_kind[ikind];
    *max_value = max_integer_value_of_kind[ikind];
  } else {
    bit_size = bit_precise_integer_width(type);
    if (bit_size > BITS_IN_AN_INTEGER_VALUE) {
      bit_size = BITS_IN_AN_INTEGER_VALUE;
    }  /* if */
    is_signed = int_kind_is_signed[ikind];
    /* Obtain the maximum value by creating a mask with the appropriate number
       of bits set.  If the integer is signed, subtract one from the bit size
       to account for the sign bit.  If the integer is unsigned, use the full
       bit size. */
    make_integer_value_mask(max_value, size_t_arg(is_signed ? bit_size - 1
                                                            : bit_size));
    if (is_signed) {
      /* Obtain the minimum value by adding one to the maximum value (relying
         on two's complement arithmetic) and sign extending the result. */
      an_integer_value one;
      a_boolean        err;
      set_integer_value(&one, (a_host_large_integer)1);
      *min_value = *max_value;
      add_integer_values(min_value, &one, /*is_signed=*/FALSE, &err);
      sign_extend_integer_value(min_value, size_t_arg(bit_size));
    } else {
      set_integer_value(min_value, (a_host_large_integer)0);
    }  /* if */
  }  /* if */
}  /* integer_value_range_for_type */


a_boolean integer_value_in_range_for_type(an_integer_value *value,
                                          a_boolean        is_signed,
                                          a_type_ptr       type)
/*
Return TRUE if value fits in the representable range of the indicated integer
type.  is_signed describes how value itself should be interpreted.
*/
{
  an_integer_value min_value, max_value;
  a_boolean        type_is_signed;
  a_boolean        in_range;

  type = skip_typerefs(type);
  check_assertion(type_is(type, tk_integer));
  if (!integer_value_can_represent_type_width(type)) {
    a_boolean type_is_unsigned =
                       !int_kind_is_signed[type->variant.integer.int_kind];
    an_integer_value zero_value;
    set_integer_value(&zero_value, (a_host_large_integer)0);
    return !type_is_unsigned || !is_signed ||
           cmp_integer_values(value, is_signed, &zero_value,
                              /*op_2_signed=*/FALSE) >= 0;
  }  /* if */
  integer_value_range_for_type(type, &min_value, &max_value);
  type_is_signed = int_kind_is_signed[type->variant.integer.int_kind];
  in_range = cmp_integer_values(value, is_signed, &min_value,
                                type_is_signed) >= 0 &&
             cmp_integer_values(value, is_signed, &max_value,
                                type_is_signed) <= 0;
  return in_range;
}  /* integer_value_in_range_for_type */


a_boolean integer_constant_in_range_for_type(a_constant *min_con,
                                             a_constant *max_con,
                                             a_type_ptr  type)
/*
Return TRUE if the range min_con..max_con falls entirely within the legal
range of values for the indicated integer type.  min_con and max_con may be
the same constant.
*/
{
  a_boolean min_con_signed, max_con_signed;
  a_boolean in_range;

  check_assertion(constant_is(min_con, ck_integer) &&
                  constant_is(max_con, ck_integer));
  min_con_signed = int_constant_is_signed(min_con);
  in_range = integer_value_in_range_for_type(&min_con->variant.integer_value,
                                             min_con_signed, type);
  if (in_range && min_con != max_con) {
    max_con_signed = int_constant_is_signed(max_con);
    in_range = integer_value_in_range_for_type(&max_con->variant.integer_value,
                                               max_con_signed, type);
  }  /* if */
  return in_range;
}  /* integer_constant_in_range_for_type */


void trim_integer_value_to_type(an_integer_value *value,
                                a_type_ptr        type)
/*
Adjust *value to fit in the value representation of the indicated integer type,
using two's-complement wrapping for unsigned and sign extension for signed.
*/
{
  an_integer_value mask;
  a_targ_size_t    bit_size;
  a_boolean        is_signed;

  type = skip_typerefs(type);
  check_assertion(type_is(type, tk_integer));
  bit_size = integer_value_bit_size_for_type(type);
  if (bit_size >= BITS_IN_AN_INTEGER_VALUE) {
    return;
  }  /* if */
  is_signed = int_kind_is_signed[type->variant.integer.int_kind];
  make_integer_value_mask(&mask, size_t_arg(bit_size));
  and_integer_values(value, &mask);
  if (is_signed) {
    sign_extend_integer_value(value, size_t_arg(bit_size));
  }  /* if */
}  /* trim_integer_value_to_type */


a_boolean le_max_integer_value_of_kind(an_integer_value *value,
                                       a_boolean	is_signed,
                                       an_integer_kind  ikind)
/*
Return TRUE if the value is less than or equal to the maximum value that
can be represented by integers of kind ikind.
*/
{
  return (cmp_integer_values(value, is_signed,
                             &max_integer_value_of_kind[ikind],
                             (a_boolean)int_kind_is_signed[ikind]) <= 0);
}  /* le_max_integer_value_of_kind */


a_boolean is_max_value_for_integer_kind(a_constant      *con,
                                        an_integer_kind ikind)
/*
Return TRUE if the integer constant's value is the maximum value for the
integer kind ikind.  This is used to check for the possibility of overflow
before doing an increment.
*/
{
  return cmp_integer_values(&con->variant.integer_value,
                            int_constant_is_signed(con),
                            &max_integer_value_of_kind[ikind],
                            (a_boolean)int_kind_is_signed[ikind]) == 0;
}  /* is_max_value_for_integer_kind */


#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
void incr_integer_value(an_integer_value *intval)
/*
Increment the integer value *intval.  No overflow checking is done.
*/
{
  an_integer_value	one;
  a_boolean		err;
  set_integer_value(&one, (a_host_large_integer)1);
  add_integer_values(intval, &one, /*is_signed=*/FALSE, &err);
}  /* incr_integer_value */
#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */


size_t bits_required_to_represent_integer_constant(a_constant *cp)
/*
Return the number of bits required to represent the indicated constant.
*/
{
  size_t            nbits = 1;
  an_integer_value  mask;
  an_integer_value  sign_mask;
  an_integer_value  value = cp->variant.integer_value;

#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  /* mask is a bit mask used to turn off the bottom bits of the value. */
  /* sign_mask is the value we expect after the bottom bits have been
     turned off.  It's the sign bit duplicated across the right number
     of bits. */
  if ((a_signed_integer_value)cp->variant.integer_value < 0 &&
      int_constant_is_signed(cp)) {
    /* Constant is negative. */
    mask = ~(an_integer_value)0;
    sign_mask = ~(an_integer_value)0;
  } else {
    /* Constant is nonnegative or unsigned. */
    mask = ~(an_integer_value)1;
    sign_mask = 0;
  }  /* if */
  /* Stop when the mask includes all the significant bits of the value. */
  while ((value & mask) != sign_mask) {
    mask <<= 1;
    sign_mask <<= 1;
    nbits++;
  }  /* while */
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  /* mask is a bit mask used to turn off the bottom bits of the value. */
  /* sign_mask is the value we expect after the bottom bits have been
     turned off.  It's the sign bit duplicated across the right number
     of bits. */
  if (sign_of(value) && int_constant_is_signed(cp)) {
    /* Constant is negative. */
    set_integer_value(&mask, (a_host_large_integer)0);
    complement_integer_value(&mask);
    sign_mask = mask;
  } else {
    /* Constant is nonnegative or unsigned. */
    set_integer_value(&mask, (a_host_large_integer)1);
    complement_integer_value(&mask);
    set_integer_value(&sign_mask, (a_host_large_integer)0);
  }  /* if */
  /* Stop when the mask includes all the significant bits of the value. */
  for (;;) {
    an_integer_value	temp;
    a_boolean		err;
    temp = value;
    and_integer_values(&temp, &mask);
    if (cmp_integer_values(&temp, /*op_1_signed=*/FALSE,
                           &sign_mask, /*op_2_signed=*/FALSE) == 0) break;
    shift_left_integer_value(&mask, 1, &err);
    shift_left_integer_value(&sign_mask, 1, &err);
    nbits++;
  }  /* for */
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  return nbits;
}  /* bits_required_to_represent_integer_constant */


void add_integer_values(an_integer_value *op_1,
			an_integer_value *op_2,
			a_boolean	 is_signed,
			a_boolean	 *err)
/*
Add two integer values.  The result is returned in the first
operand (op_1 = op_1 + op_2).  err is TRUE if an overflow occurred
and FALSE otherwise.
*/
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  a_boolean		sign_1 = sign_of(*op_1);
  an_integer_value	result;

  result = *op_1 + *op_2;
  /* Check for overflow. */
  if (!is_signed) {
    /* Check for an unsigned overflow. */
    *err = (MAX_UNSIGNED_INTEGER_VALUE - *op_1) < *op_2;    
  } else {
    /* A signed overflow occurred if the two operands have the same sign and
       the result has a different sign. */
    *err = ((sign_1 == sign_of(*op_2)) && (sign_1 != sign_of(result)));
  }  /* if */
  *op_1 = result;
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  int			i;
  a_host_large_integer	carry = 0;
  a_boolean		sign_1 = sign_of(*op_1);

  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    a_host_large_integer work;
    work = (a_host_large_integer)op_1->part[i] +
           (a_host_large_integer)op_2->part[i] + carry;
    if (work > MAX_UINT_VALUE_PART) {
      work -= (a_host_large_integer)INT_VALUE_PART_BASE;
      carry = 1;
    } else {
      carry = 0;
    }  /* if */
    op_1->part[i] = (an_int_value_part)work;
  }  /* for */
  /* Check for overflow. */
  if (!is_signed) {
    /* An unsigned overflow occurred if there was a carry out of the last
       (high-order) operation. */
    *err = (carry != 0);
  } else {
    /* A signed overflow occurred if the two operands have the same sign and
       the result has a different sign. */
    *err = ((sign_1 == sign_of(*op_2)) && (sign_1 != sign_of(*op_1)));
  }  /* if */
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* add_integer_values */


void add_mixed_signed_integer_values(an_integer_value *op_1,
				     a_boolean	      op_1_signed,
				     an_integer_value *op_2,
				     a_boolean	      op_2_signed,
				     a_boolean	      *err)
/*
Add two integer values that may be of different signedness.
The result is returned in the first operand (op_1 = op_1 + op_2).
err is TRUE if an overflow occurred and FALSE otherwise.
*/
{
  an_integer_value	orig_value;

  orig_value = op_1_signed ? *op_1 : *op_2;
  add_integer_values(op_1, op_2, op_1_signed, err);
  /* We only need special processing if the signedness of the two operands
     is different. */
  if (op_1_signed != op_2_signed) {
    /* If we are adding a signed and unsigned then the result must be
       larger than the signed value we started with.  Note that the value
       of *err determined above is ignored because it is not meaningful
       when the signedness of the operands is different.*/
    *err = cmp_integer_values(&orig_value, /*op_1_signed=*/TRUE,
                              op_1, op_1_signed) > 0;
  }  /* if */
}  /* add_mixed_signed_integer_values */


void subtract_mixed_signed_integer_values(an_integer_value *op_1,
					  a_boolean	   op_1_signed,
					  an_integer_value *op_2,
					  a_boolean	   op_2_signed,
					  a_boolean	   *err)
/*
Subtract two integer values that may be of different signedness.
The result is returned in the first operand (op_1 = op_1 + op_2).
err is TRUE if an overflow occurred and FALSE otherwise.
*/
{
  an_integer_value orig_value;

  orig_value = *op_1;
  subtract_integer_values(op_1, op_2, op_1_signed, err);
  /* We only need special processing if the signedness of the two operands
     is different.  If the second operand is positive then the result
     needs to be less than or equal to the original value; if the second
     operand is negative then the result must be greater than the original
     value.  Note that the value of *err determined above is ignored because
     it is not meaningful when the signedness of the operands is different. */
  if (op_1_signed != op_2_signed) {
    a_boolean op_2_sign = op_2_signed && sign_of(*op_2);
    int       cmp = cmp_integer_values(&orig_value, op_1_signed,
                                       op_1, op_1_signed);
    if (op_2_sign) {
      *err = cmp > 0;
    } else {
      *err = cmp <= 0;
    }  /* if */
  }  /* if */
}  /* subtract_mixed_signed_integer_values */


void make_integer_value_mask(an_integer_value *mask,
                             size_t           bits)
/*
Create a mask in which the "bits" low order bits of the integer value
are set to one.  bits must be at least one.
*/
{
#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  set_integer_value(mask, (a_host_large_integer)0);
  complement_integer_value(mask);
  shift_right_integer_value(mask, (int)(BITS_IN_AN_INTEGER_VALUE - bits),
			    /*is_signed=*/FALSE, /*sign_extend=*/FALSE);
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  /* The version that works on a host type was originally a macro.  It
     was converted to a function to work around a gcc bug that caused
     the macro to fail when an_integer_value was a long long. */
  size_t                shift_count = BITS_IN_AN_INTEGER_VALUE - bits;
  an_integer_value      result = (~(an_integer_value)0);
  result = result >> shift_count;
  *mask = result;
#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* make_integer_value_mask */

#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
void sign_extend_integer_value(an_integer_value *value,
                               size_t           bits)
/*
Sign extend an integer value.  The current value consists of "bits"
bits.  The high order bit of the field is the sign bit.
*/
{
  int       shift_bits = (int)(BITS_IN_AN_INTEGER_VALUE - bits);
  a_boolean err;
  shift_left_integer_value(value, shift_bits, &err);
  shift_right_integer_value(value, shift_bits, /*is_signed=*/TRUE,
                            /*sign_extend=*/TRUE);
}  /* sign_extend_integer_value */


void or_integer_values(an_integer_value *op_1,
		       an_integer_value *op_2)
/*
Logical OR two integer values.  The result is returned in the first
operand (op_1 = op_1 | op_2).
*/
{
  int			i;
  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    op_1->part[i] = op_1->part[i] | op_2->part[i];
  }  /* for */
}  /* or_integer_values */


void and_integer_values(an_integer_value *op_1,
		        an_integer_value *op_2)
/*
Logical AND two integer values.  The result is returned in the first
operand (op_1 = op_1 & op_2).
*/
{
  int			i;
  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    op_1->part[i] = op_1->part[i] & op_2->part[i];
  }  /* for */
}  /* and_integer_values */


void xor_integer_values(an_integer_value *op_1,
		        an_integer_value *op_2)
/*
Logical exclusive OR two integer values.  The result is returned in the first
operand (op_1 = op_1 ^ op_2).
*/
{
  int			i;
  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    op_1->part[i] = op_1->part[i] ^ op_2->part[i];
  }  /* for */
}  /* xor_integer_values */


/* Macro that will return a specified part of an integer value if the part
   number is valid, otherwise returns the value in fill_value.
   fill_value is a local variable of the functions that call this macro. */
#define get_part(value, part_gp)					\
  ((a_host_large_unsigned)						\
  (((part_gp) < 0 ||							\
    (int)(part_gp) >= (int)INT_VALUE_PARTS_PER_INTEGER_VALUE) ?		\
                                      fill_value : (value).part[part_gp])) 
#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */


void shift_left_integer_value(an_integer_value *op_1,
			      int	       op_2,
			      a_boolean	       *err)
/*
Shift an integer value left.  The result is returned in the
first operand (op_1 = op_1 << op_2).  err is TRUE if an overflow
occurred, otherwise err is FALSE.  The caller must ensure that the
shift count is a legal value.
*/
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  an_integer_value	result;
  an_integer_value      bits_lost;

  if (op_2 != 0) {
    result = *op_1 << op_2;
    bits_lost = *op_1 >> (BITS_IN_AN_INTEGER_VALUE - op_2);
    *err = (bits_lost != 0);
    *op_1 = result;
  } else {
    *err = FALSE;
  }  /* if */
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  int			i;
  int			part_offset;
  int			first_part_shift;
  int			second_part_shift;
  a_boolean		overflow = FALSE;

  /* Shift a simulated integer left by op_2 bits.  part_offset is the
     value that needs to be added to a given part number to get
     the part from which the high order bits of the new value are to
     be obtained.  The low order bits come from part_offset+1.  The
     high order bits are shifted left by first_part_shift, the low
     order bits are shifted right by second_part_shift, and the two values
     are ORed together.  For each part of the original value we determine
     whether some or all of the bits will be shifted out.  If any of the
     bits to be shifted are non-zero we set the overflow flag. */
  part_offset = op_2 / (int)BITS_IN_INT_VALUE_PART;
  first_part_shift = op_2 % (int)BITS_IN_INT_VALUE_PART;
  second_part_shift = (int)(BITS_IN_INT_VALUE_PART) - first_part_shift;
  for (i = 0; i < (int)INT_VALUE_PARTS_PER_INTEGER_VALUE; ++i) {
    a_host_large_unsigned  work;
    an_int_value_part      fill_value = 0;
    work = op_1->part[i];
    if (i < part_offset && work != 0) {
      overflow = TRUE;
    } else if (i == part_offset && ((work >> second_part_shift) != 0)) {
      overflow = TRUE;
    }  /* if */
    work = get_part(*op_1, i + part_offset) << first_part_shift |
           get_part(*op_1, i + part_offset + 1) >> second_part_shift;
    op_1->part[i] = (an_int_value_part)(work & MAX_UINT_VALUE_PART);
  }  /* for */
  *err = overflow;
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* shift_left_integer_value */


void shift_right_integer_value(an_integer_value *op_1,
			       int	        op_2,
                               a_boolean        is_signed,
			       a_boolean        sign_extend)
/*
Shift an integer value right.  The result is returned in the
first operand (op_1 = op_1 >> op_2). The caller must ensure that the
shift count is a legal value.
*/
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  if (is_signed && sign_extend && sign_of(*op_1)) {
    /* Do a signed shift right. */
    *op_1 = signed_shift_right(*op_1, op_2);
  } else {
    /* Do an unsigned shift right. */
    *op_1 = *op_1 >> op_2;
  }  /* if */
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  int			i;
  int			part_offset;
  int			first_part_shift;
  int			second_part_shift;
  an_int_value_part	fill_value;

  /* Shift a simulated integer right by op_2 bits.  part_offset is the
     value that needs to be subtracted from a given part number to get
     the part from which the low order bits of the new value are to
     be obtained.  The high order bits come from part_offset-1.  The
     high order bits are shifted right by first_part_shift, the low
     order bits are shifted left by second_part_shift, and the two values
     are ORed together. */
  part_offset = op_2 / (int)BITS_IN_INT_VALUE_PART;
  first_part_shift = op_2 % (int)BITS_IN_INT_VALUE_PART;
  second_part_shift = (int)BITS_IN_INT_VALUE_PART - first_part_shift;
  /* fill_value contains the bits to be shifted in from the left.  It is
     zero for positive numbers and -1 (0xffff...) for negative numbers. */
  if (is_signed && sign_extend) {
    fill_value = -(int)sign_of(*op_1) & MAX_UINT_VALUE_PART;
  } else {
    fill_value = 0;
  }  /* if */
  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    a_host_large_unsigned  work;
    work = get_part(*op_1, i - part_offset) >> first_part_shift |
           get_part(*op_1, i - part_offset - 1) << second_part_shift;
    op_1->part[i] = (an_int_value_part)(work & MAX_UINT_VALUE_PART);
  }  /* for */
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* shift_right_integer_value */


void subtract_integer_values(an_integer_value *op_1,
			     an_integer_value *op_2,
			     a_boolean	      is_signed,
			     a_boolean	      *err)
/*
Subtract two integer values.  The result is returned in the first
operand (op_1 = op_1 - op_2).  err is TRUE if an overflow or
underflow occurred.
*/
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  a_boolean		overflow = FALSE;
  a_boolean		sign_1 = sign_of(*op_1);
  an_integer_value	result;

  result = (a_signed_integer_value)(*op_1 - *op_2);
  /* Check overflow possibilities. */
  if (!is_signed) {
    /* A result that would yield a negative result is an error. */
    if (*op_2 > *op_1) overflow = TRUE;
  } else {
    /* A signed underflow or overflow occurred if the two operands have
       different signs and the result does not have the same sign as
       the first operand. */
    overflow = ((sign_of(*op_1) != sign_of(*op_2)) &&
                (sign_1 != sign_of(result)));
  }  /* if */
  *op_1 = result;
  *err = overflow;
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  int			i;
  a_host_large_integer	borrow = 0;
  a_boolean		sign_1 = sign_of(*op_1);

  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    a_host_large_integer work;
    work = (a_host_large_integer)op_1->part[i] -
           (a_host_large_integer)op_2->part[i] - borrow;
    if (work < 0) {
      work += (a_host_large_integer)INT_VALUE_PART_BASE;
      borrow = 1;
    } else {
      borrow = 0;
    }  /* if */
    op_1->part[i] = (an_int_value_part)work;
  }  /* for */
  /* Check for underflow or overflow. */
  if (!is_signed) {
    /* An unsigned underflow occurred if there was a borrow during the
       last (high-order) operation, which means the result would be
       negative. */
    *err = (borrow != 0);
  } else {
    /* A signed underflow or overflow occurred if the two operands have
       different signs and the result does not have the same sign as
       the first operand. */
    *err = ((sign_1 != sign_of(*op_2)) && (sign_1 != sign_of(*op_1)));
  }  /* if */
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* subtract_integer_values */


void negate_integer_value(an_integer_value *op_1,
			  a_boolean	    *err)
/*
Negate an integer value.  The result is returned in the first operand
(op_1 = -op_1).  err is TRUE if an overflow occurred.
*/
{
  an_integer_value  result;
  set_integer_value(&result, (a_host_large_integer)0);
  subtract_integer_values(&result, op_1, /*is_signed=*/TRUE, err);
  *op_1 = result;
}  /* negate_integer_value */


#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
void complement_integer_value(an_integer_value *op_1)
/*
Complement an integer value.  The result is returned in the
operand (op_1 = ~op_1).
*/
{
  int	i;

  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
    op_1->part[i] = ~op_1->part[i] & MAX_UINT_VALUE_PART;
  }  /* for */
}  /* complement_integer_value */


/*
Clear an array of integer value parts.
*/
#define clear_parts(to_arg, parts_cp)					\
  {									\
    int                i_cp;						\
    an_int_value_part  *to_cp = to_arg;					\
    for (i_cp = 0; i_cp < (int)(parts_cp); ++i_cp) to_cp[i_cp] = 0;	\
  }


/*
Copy a block of integer value parts.  The from and to arguments are addresses
of elements in an array of parts.
*/
#define copy_parts(from_arg, to_arg, parts_cp)				\
  {									\
    int                i_cp;						\
    an_int_value_part  *from_cp = from_arg;				\
    an_int_value_part  *to_cp = to_arg;					\
    for (i_cp = 0; i_cp < (int)(parts_cp); ++i_cp) {			\
      to_cp[i_cp] = from_cp[i_cp];					\
    }  /* for */							\
  }
#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */


#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
static a_signed_integer_value divide_integers(a_signed_integer_value value_1,
                		              a_signed_integer_value value_2)
/*
Divide value_1 by value_2 and return the quotient.  This routine forces
truncation toward zero on division involving negative numbers, which
is not guaranteed by C.  The caller must ensure that value_2 is not zero
and that if value_2 == -1, value_1 != MIN_INTEGER_VALUE on a twos' complement
machine.
*/
{
  a_signed_integer_value result = value_1 / value_2;

  /* If either of the values is negative, check for truncation away from zero
     and compensate for it.  Since by definition
       (value_1/value_2)*value_2 + value_1%value_2 == value_1,
     if the sign of value_1%value_2 is different than the sign of value_1
     truncation was away from zero. */
  if (value_1 < 0) {
    if (value_1 % value_2 > 0) result++;
  } else if (value_2 < 0) {
    if (value_1 % value_2 < 0) result++;
  }  /* if */
  return result;
}  /* divide_integers */
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */


void multiply_integer_values(an_integer_value *orig_op_1,
			     an_integer_value *orig_op_2,
			     a_boolean	      is_signed,
			     a_boolean	      *err)
/*
Multiply two integer values.  The result is returned in the first operand
(op_1 = op_1 * op_2).  err is TRUE if an overflow occurred and FALSE
otherwise.
*/
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  a_boolean	overflow = FALSE;

  if (!is_signed) {
    an_integer_value	op_1 = *orig_op_1;
    an_integer_value	op_2 = *orig_op_2;
    an_integer_value	result;
    result = (a_signed_integer_value)((an_integer_value)op_1 *
                                      (an_integer_value)op_2);
    *orig_op_1 = result;
    /* Check overflow possibilities. */
    /* divide_integers is not needed here, since the operation is unsigned. */
    if (op_2 != 0 && MAX_UNSIGNED_INTEGER_VALUE / op_2 < op_1) overflow = TRUE;
    *orig_op_1 = result;
  } else {
    a_signed_integer_value	op_1 = *orig_op_1;
    a_signed_integer_value	op_2 = *orig_op_2;
    a_signed_integer_value	result;
    result = (a_signed_integer_value)((an_integer_value)op_1 *
                                      (an_integer_value)op_2);
    *orig_op_1 = result;
    if (op_1 > 0 && op_2 > 0) {
      /* divide_integers is not needed here, since both numbers are
         positive. */
      if (MAX_INTEGER_VALUE / op_2 < op_1) overflow = TRUE;
    } else if (op_1 > 0 && op_2 < -1) {
      /* As an example of the kind of problem that calls for divide_integers,
         on a 32-bit twos' complement machine:
           op_1 == 715827883, op_2 == -3 -- product overflows.
           LONG_MIN == -2147483648
         on a machine where truncation is not towards zero,
           LONG_MIN / op_2 == 715827883
           LONG_MIN % op_2 == 1
         which fulfills the definition
           (LONG_MIN/op_2)*op_2 + LONG_MIN%op_2 == LONG_MIN
         but LONG_MIN/op_2 == op_1, so no error is detected. */
      if (divide_integers(MIN_INTEGER_VALUE, op_2) < op_1) overflow = TRUE;
    } else if (op_1 < -1 && op_2 > 0) {
      if (divide_integers(MIN_INTEGER_VALUE, op_1) < op_2) overflow = TRUE;
    } else if (op_1 < -1 && op_2 < 0) {
      if (divide_integers(MAX_INTEGER_VALUE, op_1) > op_2) overflow = TRUE;
    /*lint -e{506}*/
    } else if ((MIN_INTEGER_VALUE + MAX_INTEGER_VALUE) < 0 &&
               ((op_1 == -1 && op_2 == MIN_INTEGER_VALUE) ||
                (op_1 == MIN_INTEGER_VALUE && op_2 == -1))) {
      /* -1 times the smallest integer is an overflow on a 2's complement
         machine. */
      overflow = TRUE;
    }  /* if */
  }  /* if */
  *err = overflow;
  return;
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
/* The number of parts in the area where the product is initially
   computed. */
#define WORK_AREA_PARTS (INT_VALUE_PARTS_PER_INTEGER_VALUE * 2)

  an_integer_value      *op_1 = orig_op_1;
  an_integer_value      *op_2 = orig_op_2;
  an_integer_value      local_op_1;
  an_integer_value      local_op_2;
  a_boolean		negate_result = FALSE;
  an_int_value_part	work_area[WORK_AREA_PARTS];
  int			i;
  int			j;
  a_host_large_unsigned	carry = 0;
  a_boolean		overflow = FALSE;
  a_boolean		result_sign;
  a_boolean		negate_err;

  /* Clear area where the result is built. */
  clear_parts(&work_area[0], WORK_AREA_PARTS);
  /* If an operand is negative negate it and record the information so
     that it can be adjusted later.  Change the "op_x" pointer to point
     to the local copy of the negated operand.  Note that an error can
     occur when negating the most negative integer.  Although the error
     flag is set the correct value is in fact returned, so we can
     use this value for the remainder of the operations.  negate_result
     will be TRUE if either of the operands is negative and FALSE if
     both are positive or negative. */
  if (is_signed) {
    if (sign_of(*op_1)) {
      local_op_1 = *orig_op_1;
      op_1 = &local_op_1;
      negate_integer_value(op_1, err);
      negate_result = !negate_result;
    }  /* if */
    if (sign_of(*op_2)) {
      local_op_2 = *orig_op_2;
      op_2 = &local_op_2;
      negate_integer_value(op_2, err);
      negate_result = !negate_result;
    }  /* if */
  }  /* if */
  /* Multiply the two numbers.  This is essentially the multiple-precision
     multiplication algorithm from "The Art of Computer Programming", Volume
     II, by Donald Knuth, page 253. */
  for (j = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; j >= 0; --j) {
    carry = 0;
    for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1; i >= 0; --i) {
      a_host_large_integer  work_slot = i + j + 1;
      a_host_large_unsigned work_value = work_area[work_slot];
      work_value = ((a_host_large_unsigned)op_1->part[i] *
                    (a_host_large_unsigned)op_2->part[j]) + carry + work_value;
      /* The carry value is work_value divided by the size of each integer
         value part.  The value to be stored in this result part is
         work_value modulo the size of each value part. */
      carry = work_value >> BITS_IN_INT_VALUE_PART;
      work_value &= MAX_UINT_VALUE_PART;
      work_area[work_slot] = (an_int_value_part)work_value;
    }  /* for */
    work_area[j] = (an_int_value_part)carry;
  }  /* for */
  /* Copy the low order parts of the result back into the original operand
     1 provided by the caller. */
  j = INT_VALUE_PARTS_PER_INTEGER_VALUE;
  copy_parts(&work_area[j], &orig_op_1->part[0], j);
  /* If any of the high-order parts of the work area are non-zero then an
     overflow occurred. */
  for (--j; j >= 0; --j) {
    if (work_area[j] != 0) {
      overflow = TRUE;
      continue;
    }  /* if */
  }  /* for */
  /* Check for overflow into the sign bit. */
  if (is_signed) {
    result_sign = sign_of(*orig_op_1);
    if (negate_result) {
      negate_integer_value(orig_op_1, &negate_err);
    } else {
      negate_err = FALSE;
    }  /* if */
    /* The only time an error can occur while negating a number is when
       the number is the most negative integer value.  This is also the
       only case in which an overflow into the sign bit is not an error. */
    if (result_sign && !negate_err) overflow = TRUE;
  }  /* if */
  *err = overflow;
#undef WORK_AREA_PARTS
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* multiply_integer_values */


#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
static void multiply_by_int_value_part(an_int_value_part	*value,
				       int			parts,
				       an_int_value_part	factor)
/*
A simpler version of the multiply routine that can only multiply by
a single "int value part".  This is used by the division routine to
normalize the dividend and divisor.  The integer value is passed
as a pointer to the first part instead of being passed as a structure
as is normally done.  This is done to allow the division routine to pass
a pointer to its work area which is larger than a normal integer value.
*/
{
  int			i;
  a_host_large_unsigned	work;
  a_host_large_unsigned	carry = 0;

  for (i = parts - 1; i >= 0; --i) {
    work = ((a_host_large_unsigned)value[i] *
            (a_host_large_unsigned)factor) + carry;
    value[i] = (an_int_value_part)(work & MAX_UINT_VALUE_PART);
    carry = work >> BITS_IN_INT_VALUE_PART;
  }  /* for */
}  /* multiply_by_int_value_part */


static void divide_by_int_value_part(an_int_value_part	*value,
			             int		parts,
				     an_int_value_part	divisor)
/*
A simpler version of the divide routine that can only divide by a
single "int value part".  This is used by the division routine to
denormalize the remainder.  The integer value is passed as a pointer
to the first part instead of being passed as a structure as is
normally done.  This is done to allow the division routine to pass a
pointer to its work area which is larger than a normal integer value.
*/
{
  int			i;
  a_host_large_unsigned	work;
  a_host_large_unsigned	borrow = 0;

  for (i = 0; i < parts; ++i) {
    work = value[i];
    value[i] = (an_int_value_part)(((a_host_large_unsigned)value[i] + borrow) /
                                               (a_host_large_unsigned)divisor);
    borrow = ((work + borrow) % divisor) * INT_VALUE_PART_BASE;
  }  /* for */
}  /* divide_by_int_value_part */


static void special_subtract(an_int_value_part  *work_area,
                             an_int_value_part  *subtrahend)
/*
A version of the subtract routine that works with a pointer to a
part of a work area rather than a normal integer value.
*/
{
  int                  i;
  a_host_large_integer borrow = 0;

  for (i = INT_VALUE_PARTS_PER_INTEGER_VALUE; i >= 0; --i) {
    a_host_large_integer work;
    work = (a_host_large_integer)work_area[i] -
           (a_host_large_integer)subtrahend[i] - borrow;
    if (work < 0) {
      work += (a_host_large_integer)INT_VALUE_PART_BASE;
      borrow = 1;
    } else {
      borrow = 0;
    }  /* if */
    work_area[i] = (an_int_value_part)work;
  }  /* for */
}  /* special_subtract */


/* The number of parts in the area where the normalized dividend is stored. */
#define WORK_AREA_PARTS (INT_VALUE_PARTS_PER_INTEGER_VALUE * 2 + 1)

/* The number of parts in the initial quotient that is calculated. */
#define QUOTIENT_PARTS (INT_VALUE_PARTS_PER_INTEGER_VALUE + 1)


static void divide_and_remainder_integer_values(an_integer_value *orig_op_1,
					        an_integer_value *orig_op_2,
					        an_integer_value *quotient,
					        an_integer_value *remainder,
					        a_boolean	 is_signed,
					        a_boolean	 *err)
/*
Divide and compute the remainder of two simulated large integer values.

	quotient = op_1 / op_2
	remainder = op_1 % op_2

err is TRUE if an overflow occurred.

This routine is called by the divide and remainder routines that are visible
to the rest of the compiler.  Either quotient or remainder will point to the
same integer value entry as op_1 so they should not be updated until we
are done with op_1.
*/
{
  an_integer_value      *op_1 = orig_op_1;
  an_integer_value      op_2;
  an_integer_value      local_op_1;
  an_integer_value	zero_iv;
  an_int_value_part	local_quotient[QUOTIENT_PARTS];
  int			quotient_pos;
  a_boolean		negate_quotient = FALSE;
  a_boolean		negate_remainder = FALSE;
  an_int_value_part	work_area[WORK_AREA_PARTS];
  an_int_value_part	temp_product[WORK_AREA_PARTS];
  an_int_value_part	normalization_factor;
  int			i;
  int			wa_first_part = 0;
  int			wa_parts = 0;
  int			j;
  int			op_2_first_part = 0;
  int			op_2_parts = 0;
  a_boolean		overflow = FALSE;
  a_host_large_unsigned	v1;

  /* Clear the areas where intermediate results are stored. */
  clear_parts(&work_area[0], WORK_AREA_PARTS);
  clear_parts(&temp_product[0], WORK_AREA_PARTS);
  /* Clear the area where the quotient will be constructed. */
  clear_parts(&local_quotient[0], QUOTIENT_PARTS);
  /* If an operand is negative negate it and record the information so
     that it can be adjusted later.  Change the op_1 pointer to point
     to the local copy of the negated operand.  A local copy of op_2 is
     always made because we need to be able to modify it during
     normalization.   Note that an error can occur when negating the
     most negative integer.  Although the error flag is set the correct
     value is in fact returned, so we can use this value for the
     remainder of the operations.  negate_quotient will be TRUE if either
     of the operands is negative and FALSE if both are positive or
     negative.  negate_remainder is TRUE if the dividend is negative. */
  op_2 = *orig_op_2;
  if (is_signed) {
    if (sign_of(*op_1)) {
      local_op_1 = *orig_op_1;
      op_1 = &local_op_1;
      negate_integer_value(op_1, err);
      negate_quotient = !negate_quotient;
      negate_remainder = TRUE;
    }  /* if */
    if (sign_of(op_2)) {
      negate_integer_value(&op_2, err);
      negate_quotient = !negate_quotient;
    }  /* if */
  }  /* if */
  /* Check for division by zero, a zero dividend, and for cases where
     the divisor is greater than the dividend. */
  set_integer_value(&zero_iv, (a_host_large_integer)0);
  if (cmp_integer_values(&op_2, /*op_1_signed=*/FALSE, &zero_iv,
                                /*op_2_signed=*/FALSE) == 0) {
    /* Divisor is zero -- this is an error. */
    *quotient = zero_iv;
    *remainder = zero_iv;
    overflow = TRUE;
    goto exit;
  } else if (cmp_integer_values(op_1, /*op_1_signed=*/FALSE, &zero_iv,
                         /*op_2_signed=*/FALSE) == 0) {
    /* Dividend is zero.  Set quotient and remainder to zero. */
    *quotient = zero_iv;
    *remainder = zero_iv;
    goto exit;
  } else if (cmp_integer_values(op_1, /*op_1_signed=*/FALSE, &op_2,
                                /*op_2_signed=*/FALSE) < 0) {
    /* The divisor is greater than the dividend.  Set the remainder
       to the dividend and the quotient to zero. */
    *quotient = zero_iv;
    *remainder = *orig_op_1;
    goto exit;
  }  /* if */
  /* Copy the dividend to the work area.  Leave one empty part in the
     high order portion of the work area.  This may be used for
     normalization later. */
  copy_parts(&op_1->part[0], &work_area[1], INT_VALUE_PARTS_PER_INTEGER_VALUE);
  /* Compute the number of parts actually used in op_2. */
  for (j = 0; j < (int)INT_VALUE_PARTS_PER_INTEGER_VALUE; ++j) {
    if (op_2.part[j] != 0) {
      op_2_first_part = j;
      op_2_parts = (int)INT_VALUE_PARTS_PER_INTEGER_VALUE - op_2_first_part;
      break;
    }  /* if */
  }  /* for */
  /* Compute the number parts actually used in the work area. */
  for (i = 0; i <= (int)INT_VALUE_PARTS_PER_INTEGER_VALUE; ++i) {
    if (work_area[i] != 0) {
      wa_first_part = i;
      wa_parts = (int)INT_VALUE_PARTS_PER_INTEGER_VALUE - wa_first_part + 1;
      break;
    }  /* if */
  }  /* for */
  /* Do the division.  This is essentially the multiple-precision
     division algorithm from "The Art of Computer Programming", Volume
     II, by Donald Knuth, page 257.  The various steps below are
     annotated with the step numbers from the algorithm in the
     book (e.g., D2). */
  /* D1. Normalize the divisor.   This is done by dividing the base of an
     integer value part (e.g., 65536) by the first non-zero part of the
     divisor (plus 1).  The result is a factor that, when multiplied
     by the first digit of the divisor, will yield a value > (base/2). */
  normalization_factor = (an_int_value_part)((INT_VALUE_PART_BASE /
                                           (op_2.part[op_2_first_part] + 1)));
  multiply_by_int_value_part(op_2.part, INT_VALUE_PARTS_PER_INTEGER_VALUE,
                             normalization_factor);
  /* Multiply the dividend by the normalization factor. */
  multiply_by_int_value_part(work_area, WORK_AREA_PARTS, normalization_factor);
  /* D2.  Loop through the parts of op_1 (which is now in the work area).
     The number of iterations is based on the relative magnitudes of the
     two operands.  The number of iterations through the loop ends up
     being wa_parts - op_2_parts + 1. */
  v1 = op_2.part[op_2_first_part];
  quotient_pos = (int)INT_VALUE_PARTS_PER_INTEGER_VALUE -
                                                (wa_parts - op_2_parts) - 1;
  for (j = wa_first_part - 1;
       quotient_pos < (int)INT_VALUE_PARTS_PER_INTEGER_VALUE;
       ++j, quotient_pos++) {
    /* D3. Compute a trial value of the first part of the quotient. 
       The following subscripts are always legal because the work area
       contains extra elements that have been set to zero. */
    a_host_large_unsigned	u0 = work_area[j];
    a_host_large_unsigned	u1 = work_area[j + 1];
    a_host_large_unsigned 	q;
    a_host_large_unsigned	qx;
    a_boolean			done;
    if (v1 == u0 ) {
      q = MAX_UINT_VALUE_PART;
    } else {
      qx = (u0 * INT_VALUE_PART_BASE) + u1;
      q = qx / v1;
    }  /* if */
    /* D4. Compute work_area = work_area - (q * op_2). */
    /* Compute q * op_2. */
    do {
      /* Copy the divisor into a temporary work area where it can be
         multiplied by q.  The temporary area has an extra part to ensure
         that the multiply will not overflow. */
      clear_parts(&temp_product[0], WORK_AREA_PARTS);
      copy_parts(&op_2.part[op_2_first_part], &temp_product[1],
                 (int)INT_VALUE_PARTS_PER_INTEGER_VALUE - op_2_first_part);
      multiply_by_int_value_part(&temp_product[0],
                                 INT_VALUE_PARTS_PER_INTEGER_VALUE + 1,
                               (an_int_value_part)q);
      /* If the value to be subtracted is greater than the value in
         the work area, reduce the quotient by 1 and try again.  This is
         not the method used by the original algorithm.  This is
         somewhat slower but much simpler and avoids some potential
         overflow problems. */
      done = TRUE;
      for (i = 0; i < (int)(INT_VALUE_PARTS_PER_INTEGER_VALUE + 1); ++i) {
        int diff = work_area[i + j] - temp_product[i];
        if (diff == 0) continue;
        if (diff < 0 ) {
          done = FALSE;
          q--;
        }  /* if */
        break;
      }  /* for */
    } while (!done);
    /* Subtract the value calculated above from the work area. */
    special_subtract(&work_area[j], &temp_product[0]);
    /* D5. Save the first digit of the quotient. */
    local_quotient[quotient_pos] = (an_int_value_part)q;
    /* D7.  Loop on j. */
  }  /* for */
  /* D8.  Unnormalize the remainder. */
  divide_by_int_value_part(&work_area[1], INT_VALUE_PARTS_PER_INTEGER_VALUE,
                           normalization_factor);
  /* Copy the results to their final locations. */
  copy_parts(&local_quotient[0], &quotient->part[0],
             INT_VALUE_PARTS_PER_INTEGER_VALUE);
  copy_parts(&work_area[1], &remainder->part[0],
             INT_VALUE_PARTS_PER_INTEGER_VALUE);
  /* Adjust the sign of the results.  Negating the remainder cannot
     result in an overflow. */
  if (negate_remainder) {
    negate_integer_value(remainder, err);
  }  /* if */
  /* The only division case that produces an overflow (other than
     division by zero) is MIN_INT / -1.  In this case negate_quotient
     will be FALSE and the error will be detected by determining that
     the sign of the quotient has become negative.  MIN_INT / 1 is
     legal but will produce an overflow when negate_integer_value
     is called.  This overflow is meaningless and is ignored. */
  if (negate_quotient) {
    negate_integer_value(quotient, err);
  } else if (is_signed && sign_of(*quotient)) {
    overflow = TRUE;
  }  /* if */
  /* Special cases and error cases branch here. */
exit:
  *err = overflow;
}  /* divide_and_remainder_integer_values */
#undef WORK_AREA_PARTS
#undef QUOTIENT_PARTS
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */


void divide_integer_values(an_integer_value *op_1,
			   an_integer_value *op_2,
			   a_boolean	    is_signed,
			   a_boolean	    *err)
/*
Divide two integer values.  The result is returned in the first
operand (op_1 = op_1 / op_2).  err is TRUE if an overflow occurred.
*/
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  a_signed_integer_value	result;
  a_signed_integer_value	value_1 = *op_1;
  a_signed_integer_value	value_2 = *op_2;
  a_boolean			overflow = FALSE;

  if (value_2 == 0) {
    result = 0;
    overflow = TRUE;
  } else {
    if (is_signed) {
      /* Division of signed integers. */
      /* Check for overflow possibility on a twos' complement machine. */
      /*lint -e{506}*/
      if ((MAX_INTEGER_VALUE + MIN_INTEGER_VALUE) < 0 &&
          value_1 == MIN_INTEGER_VALUE && value_2 == -1) {
        /* Smallest integer / -1 -- Overflow on 2's complement machines. */
        overflow = TRUE;
        result = value_1;
      } else {
        /* No overflow. */
        if (c99_mode) {
          /* In C99 mode, make sure the division is done with truncation
             toward zero. */
          result = divide_integers(value_1, value_2);
        } else {
          result = value_1 / value_2;
        }  /* if */
      }  /* if */
    } else {
      /* Division of unsigned integers. */
      result = (a_signed_integer_value)((an_integer_value)value_1 /
				        (an_integer_value)value_2);
    }  /* if */
  }  /* if */
  *op_1 = result;
  *err = overflow;
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  an_integer_value	remainder;
  divide_and_remainder_integer_values(op_1, op_2, op_1, &remainder,
                                      is_signed, err);
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* divide_integer_values */


void remainder_integer_values(an_integer_value *op_1,
			      an_integer_value *op_2,
			      a_boolean	       is_signed,
			      a_boolean	       *err)
/*
Compute the remainder from dividing integer values.
The result is returned in the first operand (op_1 = op_1 % op_2).
*/
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  a_signed_integer_value	result;
  a_signed_integer_value	value_1 = *op_1;
  a_signed_integer_value	value_2 = *op_2;
  a_boolean			overflow = FALSE;

  if (value_2 == 0) {
    result = 0;
    overflow = TRUE;
  } else {
    if (is_signed) {
      /* Remainder on signed integers. */
      if (value_2 == -1) {
        /* x % -1 is always 0.  Done as a special case to avoid potential
           problems when evaluating smallest-int % -1 on a two's complement
           machine.  The corresponding division overflows, but % is
           well-defined. */
        result = 0;
      } else {
        /* No overflow. */
        result = value_1 % value_2;
      }  /* if */
    } else {
      /* Remainder on unsigned integers. */
      result = (a_signed_integer_value)((an_integer_value)value_1 %
				        (an_integer_value)value_2);
    }  /* if */
  }  /* if */
  *op_1 = result;
  *err = overflow;
#else /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  an_integer_value	quotient;
  an_integer_value	minus_one;

  set_integer_value(&minus_one, (a_host_large_integer)-1);
  if (cmp_integer_values(op_2, is_signed,
                         &minus_one, /*op_2_signed=*/TRUE) == 0) {
    /* x % -1 is always 0.  Done as a special case to avoid potential
       problems when evaluating smallest-int % -1 on a two's complement
       machine.  The corresponding division overflows, but % is
       well-defined. */
    set_integer_value(op_1, (a_host_large_integer)0);
    *err = FALSE;
  } else {
    /* No overflow. */
    divide_and_remainder_integer_values(op_1, op_2, &quotient, op_1,
                                      is_signed, err);
  }  /* if */
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}  /* remainder_integer_values */

/* Append a host integer without narrowing configurations that use 128-bit
   host integers. */
static void append_host_unsigned(a_string *result,
                                  a_host_large_unsigned value)
{
  char digits[sizeof(value) * CHAR_BIT + 1];
  unsigned count = unsigned_to_string_buf(value, digits);
  edg_string_append_view(result, edg_string_view(digits, count));
}

static void append_integer_hex(a_string *result, an_integer_value *value,
                                a_targ_size_t size)
{
  static const char hex_digits[] = "0123456789abcdef";
  char digits[(BITS_IN_AN_INTEGER_VALUE + 3) / 4];
  size_t count = 0, limit = (size_t)((size * targ_char_bit) / 4);
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  an_integer_value work = *value;
  do {
    digits[count++] = hex_digits[work & 15];
    work >>= 4;
  } while (work != 0);
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  size_t first = 0;
  check_assertion(MAX_UINT_VALUE_PART == 0xffff);
  while (first + 1 < INT_VALUE_PARTS_PER_INTEGER_VALUE &&
         value->part[first] == 0) ++first;
  for (size_t i = INT_VALUE_PARTS_PER_INTEGER_VALUE; i > first;) {
    an_int_value_part part = value->part[--i];
    unsigned n = 4;
    do {
      digits[count++] = hex_digits[part & 15];
      part >>= 4;
    } while (--n != 0 && (i != first || part != 0));
  }
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  edg_string_append_cstr(result, "0x");
  if (count > limit) count = limit;
  edg_string_reserve(result, result->len + count);
  while (count != 0) edg_string_append_char(result, digits[--count]);
}

static void append_integer_unsigned(a_string *result, an_integer_value *value)
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  append_host_unsigned(result, (a_host_large_unsigned)*value);
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  a_host_large_unsigned max_power_of_10 = 1;
  size_t chunk_digits = 0;
  a_host_large_unsigned parts[INT_VALUE_PARTS_PER_INTEGER_VALUE];
  an_integer_value value_copy = *value, divisor;
  size_t first = INT_VALUE_PARTS_PER_INTEGER_VALUE - 1;
  /* Choose the same largest signed-host power of ten used by the original
     formatter, avoiding both floating math and a narrowing host conversion. */
  while (max_power_of_10 <= (a_host_large_unsigned)MAX_HOST_LARGE_INTEGER / 10) {
    max_power_of_10 *= 10;
    ++chunk_digits;
  }
  set_unsigned_integer_value(&divisor, max_power_of_10);
  for (;;) {
    a_host_large_integer part;
    a_boolean err;
    if (cmp_integer_values(&value_copy, FALSE, &divisor, FALSE) <= 0) {
      conv_integer_value_to_host_large_integer(&value_copy, FALSE, &part, &err);
      check_assertion(!err);
      parts[first] = (a_host_large_unsigned)part;
      break;
    } else {
      an_integer_value remainder;
      divide_and_remainder_integer_values(&value_copy, &divisor, &value_copy,
                                           &remainder, FALSE, &err);
      check_assertion(!err);
      conv_integer_value_to_host_large_integer(&remainder, FALSE, &part, &err);
      check_assertion(!err && first != 0);
      parts[first--] = (a_host_large_unsigned)part;
    }
  }
  append_host_unsigned(result, parts[first]);
  for (++first; first < INT_VALUE_PARTS_PER_INTEGER_VALUE; ++first) {
    char digits[sizeof(a_host_large_unsigned) * CHAR_BIT + 1];
    size_t count = unsigned_to_string_buf(parts[first], digits);
    for (size_t i = count; i < chunk_digits; ++i)
      edg_string_append_char(result, '0');
    edg_string_append_view(result, edg_string_view(digits, count));
  }
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}

static void append_integer_signed(a_string *result, an_integer_value *value)
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  a_host_large_integer signed_value = (a_host_large_integer)*value;
  a_host_large_unsigned magnitude = (a_host_large_unsigned)signed_value;
  if (signed_value < 0) {
    edg_string_append_char(result, '-');
    magnitude = (a_host_large_unsigned)0 - magnitude;
  }
  append_host_unsigned(result, magnitude);
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  an_integer_value work = *value;
  if (sign_of(work)) {
    a_boolean err;
    edg_string_append_char(result, '-');
    negate_integer_value(&work, &err);
    if (err) {
      an_integer_value remainder, divisor;
      work = *value;
      set_integer_value(&divisor, (a_host_large_integer)10);
      divide_and_remainder_integer_values(&work, &divisor, &work, &remainder,
                                           TRUE, &err);
      check_assertion(!err);
      negate_integer_value(&work, &err);
      check_assertion(!err);
      negate_integer_value(&remainder, &err);
      check_assertion(!err);
      append_integer_unsigned(result, &work);
      append_integer_unsigned(result, &remainder);
      return;
    }
  }
  append_integer_unsigned(result, &work);
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}

a_number_buffer str_for_integer_value(an_integer_value *p_value,
                                      a_boolean        is_signed,
                                      a_boolean        non_arithmetic,
                                      a_targ_size_t    size)
/*
Return the literal form of the integer value *p_value as a string.  is_signed
indicates whether the value should be treated as signed.  A TRUE value for
non_arithmetic indicates that the constant should be considered as a bit mask
or the like instead of a number and thus should be represented as a hexadecimal
literal.  size is the number of target bytes in the value's type.  If an
arithmetic value is negative, it is preceded by a "-".
*/
{
  a_number_buffer result;
  edg_string_init_small(&result, EDG_ALLOC_GENERAL);

  if (non_arithmetic) {
    /* The constant is to be considered as a bit mask or the like, i.e., it was
       originally specified in hexadecimal or octal or it was folded from
       bit-manipulation expressions.  Put it out in hexadecimal. */
    append_integer_hex(&result, p_value, size);
  } else {
    /* Put out the value in decimal. */
    if (is_signed) {
      append_integer_signed(&result, p_value);
    } else {
      append_integer_unsigned(&result, p_value);
    }  /* if */
  }  /* if */
  return result;
}  /* str_for_integer_value */


a_number_buffer str_for_self_contained_integer_value(an_integer_value *value)
/*
Interface to str_for_integer_value for integer values whose interpretation is
wholly self-contained.
*/
{
  return str_for_integer_value(value, sign_of(*value),
                               /*non_arithmetic=*/FALSE,
                               sizeof(an_integer_value));
}  /* str_for_integer_value */


a_number_buffer str_for_integer_constant(a_constant *cp)
/*
Interface to str_for_integer_value that extracts the value, signedness,
size, and whether the value should be considered as numeric or as a bit
mask from the constant pointed to by cp.
*/
{
  return str_for_integer_value(&cp->variant.integer_value,
                               int_constant_is_signed(cp),
                               cp->non_arithmetic,
                               skip_typerefs(cp->type)->size);
}  /* str_for_integer_constant */


a_number_buffer decimal_str_for_integer_constant(a_constant *cp)
/*
Interface to str_for_integer_value that extracts the value, signedness, and
size from the constant pointed to by cp, forcing a decimal representation.
*/
{
  return str_for_integer_value(&cp->variant.integer_value,
                               int_constant_is_signed(cp),
                               /*non_arithmetic=*/FALSE,
                               skip_typerefs(cp->type)->size);
}  /* decimal_str_for_integer_constant */

#if !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER || FIXED_POINT_ALLOWED

void conv_float_string_to_integer_value(a_const_char		*float_str,
					an_integer_value	*intval,
					a_boolean		is_signed,
					a_boolean		*err)
/*
Convert a string representation of a floating-point number of the form

	[ + | - ] digits . digits [ e | E [ + | - ] exp ]

to an integer value.  Any fractional part of the floating-point number
is discarded.  This routine assumes that the string is properly formatted
so no checking is done.
*/
{
  char			digit_string[50];
  int			digits = 0;
  a_boolean		decimal_present = FALSE;
  int			digits_before_decimal = 0;
  char			*digit_pos = digit_string;
  a_const_char		*curr_pos = float_str;
  a_boolean		is_negative = FALSE;
  a_boolean		exp_is_negative = FALSE;
  int			exponent = 0;
  char			ch;
  a_boolean		overflow = FALSE;
  an_integer_value	ten;
  int			i;

  /* Check for an explicit sign. */
  if (*curr_pos == '+') {
    curr_pos++;
  } else if (*curr_pos == '-') {
    is_negative = TRUE;
    curr_pos++;
  }  /* if */

  /* Copy the digits to the local buffer. */
  for (;;) {
    ch = *curr_pos++;
    if (isdigit((unsigned char)ch)) {
      *digit_pos++ = ch;
      digits++;
    } else if (ch == '.') {
      decimal_present = TRUE;
      digits_before_decimal = digits;
    } else {
      break;
    }  /* if */
  }  /* for */
  *digit_pos = '\0';
  /* If no decimal point was found assume one at the end of the string
     of digits. */
  if (!decimal_present) digits_before_decimal = digits;
  /* Check for an exponent. */
  if (ch == 'E' || ch == 'e') {
    ch = *curr_pos;
    /* Check for a sign for the exponent. */
    if (ch == '+') {
      curr_pos++;
    } else if (ch == '-') {
      exp_is_negative = TRUE;
      curr_pos++;
    }  /* if */
    /* Convert the exponent to an integer. */
    for (;;) {
      ch = *curr_pos++;
      if (ch == '\0') break;
      exponent = (exponent * 10) + (ch - '0');
    }  /* for */
    if (exp_is_negative) exponent = -exponent;
  }  /* if */
  /* Determine the location of the start of any fractional component
     and adjust the number of digits to discard the digits after the decimal
     point.  What we are left with is a string of digits that will all
     be part of the final integer value.  Adjust the exponent to reflect
     the fact that the position of the decimal point has moved from its
     explicit location to its implicit location at the end of the digit
     string. */
  digits_before_decimal += exponent;
  if (digits_before_decimal < digits) {
    /* If the number of digits before the decimal is less than the
       number of digits in the string use the lower number as the
       number of digits to actually process. */
    digits = digits_before_decimal;
    exponent = 0;
  } else {
    /* Adjust the exponent to reflect the new implied decimal position. */
    exponent = digits_before_decimal - digits;
  }  /* if */
  /* Convert the string to an integer value. */
  set_integer_value(intval, (a_host_large_integer)0);
  set_integer_value(&ten, (a_host_large_integer)10);
  for (i = 0; i < digits; ++i) {
    an_integer_value	digit_iv;
    set_integer_value(&digit_iv, (a_host_large_integer)digit_string[i] - '0');
    multiply_integer_values(intval, &ten, /*is_signed=*/TRUE, &overflow);
    if (overflow) break;
    if (is_signed && is_negative) {
      subtract_integer_values(intval, &digit_iv, is_signed, &overflow);
    } else {
      add_integer_values(intval, &digit_iv, is_signed, &overflow);
    }  /* if */
    if (overflow) break;
  }  /* for */
  /* Adjust the result by the exponent. */
  if (!overflow) {
    for (i = 0; i < exponent; ++i) {
      multiply_integer_values(intval, &ten, is_signed, &overflow);
      if (overflow) break;
    }  /* for */
  }  /* if */
  *err = overflow || (!is_signed && is_negative);
}  /* conv_float_string_to_integer_value */

#endif /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER || FIXED_POINT_ALLOWED */

unsigned f_unsigned_to_string_buf(a_host_large_unsigned val,
                                  char                  *buf)
/*
Represent val as a sequence of decimal digits followed by a null character
starting at buf[0].  Return the number of digits output (not including the
final null character).
*/
{
  int h, k, l = 0;

  /* Produce the digits starting with the least significant. */
  do {
    buf[l] = '0' + (char)(val % 10);
    l += 1;
    val /= 10;
  } while (val != 0);
  buf[l] = '\0';
  /* Reverse the sequence. */
  h = l/2;
  l -= 1;
  for (k = 0; k < h; ++k) {
    char t = buf[k];
    buf[k] = buf[l-k];
    buf[l-k] = t;
  }  /* if */
  check_assertion(l + 1 >= 0);
  return (unsigned)(l + 1);
}  /* f_unsigned_to_string_buf */

#if DEBUG

a_number_buffer db_format_integer_value(an_integer_value  *value)
/*
Formats an integer value as hexadecimal.  Returns a number buffer containing
the formatted string.
*/
{
  a_number_buffer buffer;
  edg_string_init_small(&buffer, EDG_ALLOC_GENERAL);
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  append_integer_hex(&buffer, value, sizeof(*value));
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  edg_string_append_cstr(&buffer, "0x");
  for (int i = 0; i < (int)INT_VALUE_PARTS_PER_INTEGER_VALUE; ++i) {
    edg_string_append_hex(&buffer, (unsigned int)value->part[i], 0, '0');
  }
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  return buffer;
}  /* db_format_integer_value */


void db_signed_integer_value(an_integer_value  *value)
/*
Print a signed integer value representation in decimal form.
*/
{
  a_host_large_integer  host_val;
  a_boolean             err;

  conv_integer_value_to_host_large_integer(
                                 value, /*is_signed=*/TRUE, &host_val, &err);
  (void)fprintf(f_debug, "%ld %s\n", (long)host_val, err ? "(Error)" : "");
}  /* db_signed_integer_value */
#endif /* DEBUG */


void get_integer_size_and_alignment(an_integer_kind  ikind,
                                    a_targ_size_t    *p_size,
                                    a_targ_alignment *p_alignment)
/*
Determine and return the size and alignment of the target integer type
of the indicated kind.
*/
{
  a_targ_size_t    size = 0;
  a_targ_alignment alignment = 0;

  switch (ikind) {
    case ik_char:
    case ik_signed_char:
    case ik_unsigned_char:
      size = 1;
      alignment = 1;
      break;
    case ik_short:
    case ik_unsigned_short:
      size = targ_sizeof_short;
      alignment = targ_alignof_short;
      break;
    case ik_int:
    case ik_unsigned_int:
      size = targ_sizeof_int;
      alignment = targ_alignof_int;
      break;
    case ik_long:
    case ik_unsigned_long:
      size = targ_sizeof_long;
      alignment = targ_alignof_long;
      break;
#if LONG_LONG_ALLOWED
    case ik_long_long:
    case ik_unsigned_long_long:
      size = targ_sizeof_long_long;
      alignment = targ_alignof_long_long;
      break;
#endif /* LONG_LONG_ALLOWED */
#if INT128_EXTENSIONS_ALLOWED
    case ik_int128:
    case ik_unsigned_int128:
      size = targ_sizeof_int128;
      alignment = targ_alignof_int128;
      break;
#endif /* INT128_EXTENSIONS_ALLOWED */
    default:
      unexpected_condition_str(
                           "get_integer_size_and_alignment: bad integer kind");
  }  /* switch */
  *p_size = size;
  *p_alignment = alignment;
}  /* get_integer_size_and_alignment */


void get_bit_precise_integer_size_and_alignment(a_targ_size_t    bit_width,
                                                a_targ_size_t    *p_size,
                                                a_targ_alignment *p_alignment)
/*
Determine and return the size and alignment of a bit-precise integer type
with the indicated width.
*/
{
  a_targ_size_t     size;
  a_targ_alignment  alignment;

  check_assertion(bit_width != 0);
  size = (bit_width + targ_char_bit - 1) / targ_char_bit;
  if (size <= 1) {
    alignment = 1;
  } else if (size <= targ_sizeof_short) {
    alignment = targ_alignof_short;
  } else if (size <= targ_sizeof_int) {
    alignment = targ_alignof_int;
  } else if (size <= targ_sizeof_long) {
    alignment = targ_alignof_long;
#if LONG_LONG_ALLOWED
  } else if (size <= targ_sizeof_long_long) {
    alignment = targ_alignof_long_long;
#endif /* LONG_LONG_ALLOWED */
#if INT128_EXTENSIONS_ALLOWED
  } else if (size <= targ_sizeof_int128) {
    alignment = targ_alignof_int128;
#endif /* INT128_EXTENSIONS_ALLOWED */
  } else {
    alignment = targ_alignof_long;
#if LONG_LONG_ALLOWED
    alignment = targ_alignof_long_long;
#endif /* LONG_LONG_ALLOWED */
#if INT128_EXTENSIONS_ALLOWED
    alignment = targ_alignof_int128;
#endif /* INT128_EXTENSIONS_ALLOWED */
  }  /* if */
  *p_size = size;
  *p_alignment = alignment;
}  /* get_bit_precise_integer_size_and_alignment */


an_integer_kind int_kind_for_size_and_alignment(a_targ_size_t    size,
                                                a_targ_alignment alignment,
                                                a_boolean        is_signed)
/*
Return the integer kind that corresponds to the given size and alignment,
signed if is_signed is TRUE, unsigned otherwise.  If there is no such
integer kind, return ik_last.
*/
{
  an_integer_kind  int_kind;
  a_targ_size_t    int_size;
  a_targ_alignment int_alignment;
  a_boolean        int_signed;

  for (int_kind = (an_integer_kind)0;
       (int)int_kind < (int)ik_last;
       int_kind = (an_integer_kind)((int)int_kind + 1)) {
    if (is_bit_precise_kind(int_kind)) {
      continue;
    }  /* if */
    get_integer_size_and_alignment(int_kind, &int_size, &int_alignment);
    int_signed = int_kind_is_signed[(int)int_kind];
    if (int_size == size && int_alignment == alignment &&
        int_signed == is_signed) {
      /* This is the kind to use. */
      break;
    }  /* if */
  }  /* for */
  return int_kind;
}  /* int_kind_for_size_and_alignment */


#if MICROSOFT_EXTENSIONS_ALLOWED || GNU_EXTENSIONS_ALLOWED || IA64_ABI
/* The following routine is called to map Microsoft keywords __int16,
__int32, and __int64 to the appropriate int kind.  It is also used to
map GNU type modes (e.g., QI) to an appropriate int kind. */

an_integer_kind int_kind_for_bit_size(unsigned int  number_of_bits,
                                      a_boolean     is_signed)
/*
Return the integer kind that corresponds to the given number_of_bits and
signedness.  If none is found, ik_none is returned.
*/
{
  an_integer_kind  int_kind;
  a_targ_size_t    size, int_size;
  a_targ_alignment int_alignment;

  /* Compute the size in bytes, making sure no bits are lost. */
  size = number_of_bits / targ_char_bit;
  if (number_of_bits == size * targ_char_bit) {
    if (gnu_mode) {
      /* In some configurations (notably where sizeof(int) == sizeof(short)),
         GNU prefers "int" over "short", so check the appropriate "int" kind
         to see if it has the desired size before doing a search. */
      int_kind = (an_integer_kind)(is_signed ? ik_int : ik_unsigned_int);
      get_integer_size_and_alignment(int_kind, &int_size, &int_alignment);
      if (int_size == size) {
        goto have_kind;
      }  /* if */
    }  /* if */
    for (int_kind = (an_integer_kind)0;
         (int)int_kind < (int)ik_last;
         int_kind = (an_integer_kind)((int)int_kind + 1)) {
      if (is_bit_precise_kind(int_kind)) {
        continue;
      }  /* if */
      get_integer_size_and_alignment(int_kind, &int_size, &int_alignment);
      if (int_size == size &&
          int_kind_is_signed[(int)int_kind] == is_signed &&
          !(gnu_mode && int_kind == (an_integer_kind)ik_char)) {
        /* This is the kind to use.  Note: For GNU modes prefer "signed char"
           over plain "char". */
        goto have_kind;
      }  /* if */
    }  /* for */
  }  /* if */
  /* Getting here means no integer kind matches the specified size and
     signedness. */
  int_kind = (an_integer_kind)ik_none;
have_kind:;
  return int_kind;
}  /* int_kind_for_bit_size */

#endif /* MICROSOFT_EXTENSIONS_ALLOWED || GNU_EXTENSIONS_ALLOWED || IA64_ABI */
#if BUILTIN_FUNCTIONS_ENABLED

a_boolean swap_bytes_in_unsigned_integer(unsigned int     bytes,
                                         an_integer_value *value,
                                         an_integer_value *swapped)
/*
Does a byte swapping operation on the unsigned integer "value", returning
the result in "swapped".  "bytes" is the number of bytes to be swapped.
Returns TRUE unless "bytes" is larger than the size of the largest integer.
Assumes bytes are 8 bits.
*/
{
  a_boolean             err, result = TRUE;
  an_integer_value      copy_val, one_byte_val, one_byte_mask_val;

  if (bytes > TARG_SIZEOF_LARGEST_INTEGER) {
    /* Can't swap an integer larger than the largest. */
    result = FALSE;
  } else {
    check_assertion(targ_char_bit == 8);
    set_unsigned_integer_value(swapped, (a_host_large_unsigned)0);
    copy_val = *value;
    set_unsigned_integer_value(&one_byte_mask_val,
                               (a_host_large_unsigned)0xFF);
    while (bytes--) {
      one_byte_val = one_byte_mask_val;
      and_integer_values(&one_byte_val, &copy_val);
      shift_left_integer_value(&one_byte_val, (int)(bytes * 8), &err);
      check_assertion(!err);
      or_integer_values(swapped, &one_byte_val);
      shift_right_integer_value(&copy_val, 8, /*is_signed=*/FALSE,
                                /*sign_extend=*/FALSE);
    }  /* while */
  }  /* if */
  return result;
}  /* swap_bytes_in_unsigned_integer */

#endif /* BUILTIN_FUNCTIONS_ENABLED */

a_boolean conv_bytes_to_integer_value(an_integer_value *value,
                                      char             *bytes,
                                      size_t           num_bytes)
/*
Given an array of bytes that represent a host integer of arbitrary size,
encode those bytes into an_integer_value (*value).  Returns TRUE
unless num_bytes is larger than the largest integer or an error is
encountered.
*/
{
  a_boolean        result = TRUE, err;
  an_integer_value byte_val;

  if (num_bytes > targ_sizeof_largest_integer) {
    result = FALSE;
  } else {
    set_unsigned_integer_value(value, (a_host_large_unsigned)0);
    if (host_little_endian) {
      bytes += num_bytes - 1;
    }  /* if */
    for (; num_bytes > 0; --num_bytes, host_little_endian ? --bytes : ++bytes){
      a_host_large_unsigned bval = (unsigned char)*bytes;
      set_unsigned_integer_value(&byte_val, bval);
      shift_left_integer_value(value, CHAR_BIT, &err);
      result = result && !err;
      or_integer_values(value, &byte_val);
    }  /* for */
  }  /* if */
  return result;
}  /* conv_bytes_to_integer_value */


static void init_int_kind_min_max_values(an_integer_kind ikind)
/*
Initialize the elements of min_integer_value_of_kind and
max_integer_value_of_kind to contain the minimum and maximum values
for the integer kind ikind.
*/
{
  a_targ_size_t    size;
  a_targ_size_t    bit_size;
  a_targ_alignment alignment;
  a_boolean	   is_signed;

  /* Get the attributes of the integer kind. */
  if (is_bit_precise_kind(ikind)) {
    get_bit_precise_integer_size_and_alignment(bitint_maxwidth_value,
                                               &size, &alignment);
  } else {
    get_integer_size_and_alignment(ikind, &size, &alignment);
  }  /* if */
  /* Build the actual maximum and minimum values.  We do this by
     constructing a mask with "bit_size" bits set.  This is the maximum
     value.  We add one to this to get the bit pattern for the
     minimum signed value.  The minimum unsigned value is always zero. */
  bit_size = size * targ_char_bit;
  is_signed = int_kind_is_signed[ikind];
  if (is_signed) bit_size--;
  make_integer_value_mask(&max_integer_value_of_kind[ikind],
                          size_t_arg(bit_size));
  if (is_signed) {
    an_integer_value one;
    a_boolean	   err;
    set_integer_value(&one, (a_host_large_integer)1);
    min_integer_value_of_kind[ikind] = max_integer_value_of_kind[ikind];
    add_integer_values(&min_integer_value_of_kind[ikind], &one,
                      /*is_signed=*/FALSE, &err);
    sign_extend_integer_value(&min_integer_value_of_kind[ikind], bit_size + 1);
  } else {
    set_integer_value(&min_integer_value_of_kind[ikind],
                      (a_host_large_integer)0);
  }  /* if */
}  /* init_int_kind_min_max_values */


void const_ints_init(void)
/*
Initialize static variables related to const_ints.c.
*/
{
  a_byte ikind;

  /* Initialize the arrays of minimum and maximum values for the various
     integer kinds. */
  for (ikind = ik_char; ikind < ik_last; ikind++) {
    init_int_kind_min_max_values((an_integer_kind)ikind);
  }  /* for */
}  /* const_ints_init */

static void integer_translator_init(Integer_translator *value,
                                    size_t part_bytes, a_boolean uint32_parts)
{
  value->remainder = 0;
  value->part_bytes = part_bytes;
  value->uint32_parts = uint32_parts;
  edg_array_init_buffer(&value->parts, part_bytes, EDG_ALLOC_GENERAL, 0,
                        NULL, NULL, &value->buffer,
                        sizeof(value->buffer) / part_bytes);
}

void integer_value_translator_init(an_integer_value_translator *value)
{
  integer_translator_init(value, sizeof(an_integer_translator_part), FALSE);
}

/* Shift by bytes as numbers rather than copying their memory representation:
   the source and destination stream order is independent of host endianness. */
static void integer_translator_add_part(Integer_translator *value,
                                         a_host_large_unsigned part,
                                         size_t source_bytes)
{
  for (size_t i = source_bytes; i != 0;) {
    unsigned byte = (unsigned)((part >> (8 * --i)) & 0xff);
    if (value->remainder == 0) {
      edg_array_resize(&value->parts, value->parts.len + 1);
      value->remainder = value->part_bytes;
    }
    --value->remainder;
    if (value->uint32_parts) {
      uint32_t *dest = edg_array_at(&value->parts, value->parts.len - 1);
      *dest |= (uint32_t)byte << (value->remainder * 8);
    } else {
      an_integer_translator_part *dest =
                             edg_array_at(&value->parts, value->parts.len - 1);
      *dest |= (an_integer_translator_part)byte << (value->remainder * 8);
    }
  }
}

void integer_translator_add_u32(Integer_translator *value, uint32_t part)
{
  integer_translator_add_part(value, (a_host_large_unsigned)part, sizeof(part));
}

uint32_t integer_translator_get_u32(const Integer_translator *value,
                                     size_t index)
{
  check_assertion(value->uint32_parts);
  return *(const uint32_t *)edg_array_at(&value->parts, index);
}

void integer_translator_destroy(Integer_translator *value)
{
  edg_array_destroy(&value->parts);
}

a_boolean has_translator_overflowed(
                                    const an_integer_value_translator *value)
{
  return value->parts.len > sizeof(an_integer_value) /
                              sizeof(an_integer_translator_part);
}

an_integer_value as_integer_value(const an_integer_value_translator *value)
{
  an_integer_value result;
  check_assertion(!value->uint32_parts && !has_translator_overflowed(value));
  set_unsigned_integer_value(&result, (a_host_large_unsigned)0);
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  if (value->parts.len != 0)
    result = *(const an_integer_translator_part *)edg_array_at(&value->parts, 0);
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  for (size_t i = 0; i < value->parts.len; ++i)
    result.part[i] =
               *(const an_integer_translator_part *)edg_array_at(&value->parts, i);
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  return result;
}

void integer_translator_add_integer_value(Integer_translator *result,
                                           const an_integer_value *value)
{
#if INTEGER_VALUE_REPR_IS_A_HOST_INTEGER
  integer_translator_add_part(result, *value, sizeof(*value));
#else /* !INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
  for (size_t i = 0; i < INT_VALUE_PARTS_PER_INTEGER_VALUE; ++i)
    integer_translator_add_part(result, value->part[i], sizeof(value->part[i]));
#endif /* INTEGER_VALUE_REPR_IS_A_HOST_INTEGER */
}

void integer_value_as_translator(Integer_translator *result,
                                  const an_integer_value *value)
{
  integer_translator_init(result, sizeof(uint32_t), TRUE);
  integer_translator_add_integer_value(result, value);
}

/* Conditionally close the "edg" namespace. */
END_EDG_NAMESPACE

