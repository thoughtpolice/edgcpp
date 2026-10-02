/* Primitive operations not provided by QBE's scalar instruction set.
 * This file is compiled by the native compiler as well as the bootstrap seed.
 * Wide types below are used only for storage and genuine system libgcc calls.
 */
#include <stdbool.h>
#include <fenv.h>
#include <stdint.h>
#include <string.h>

typedef __int128 i128;
typedef unsigned __int128 u128;
_Static_assert(sizeof(i128) == 16, "runtime requires 128-bit integers");
_Static_assert(sizeof(long double) == 16 && __LDBL_MANT_DIG__ == 113,
               "runtime requires IEEE binary128 long double");

typedef struct { uint64_t lo, hi; } limbs128;

/* Narrow values cross the QBE/runtime boundary as unsigned object bits.
 * No _Float16 operation appears here: that would call this runtime again
 * when it is self-compiled. Binary64-to-narrow rounding is direct, including
 * values immediately on either side of a binary16/bfloat16 midpoint. */
float __edg_qbe_narrow_to_float(uint32_t value, uint32_t bfloat) {
  uint32_t bits;
  if (bfloat) {
    bits = (value & 65535) << 16;
    if ((bits & UINT32_C(0x7f800000)) == UINT32_C(0x7f800000) &&
        (bits & UINT32_C(0x007fffff))) {
      if (!(bits & UINT32_C(0x00400000))) feraiseexcept(FE_INVALID);
      bits |= UINT32_C(0x00400000);
    }
  } else {
    unsigned exponent = (value >> 10) & 31;
    unsigned fraction = value & 1023;
    bits = (value & 32768) << 16;
    if (exponent == 31) {
      bits |= UINT32_C(0x7f800000) | (fraction << 13);
      if (fraction) {
        if (!(fraction & 512)) feraiseexcept(FE_INVALID);
        bits |= UINT32_C(0x00400000);
      }
    } else if (exponent) {
      bits |= ((exponent + 112) << 23) | (fraction << 13);
    } else if (fraction) {
      unsigned shift = 0;
      while (!(fraction & 1024)) { fraction <<= 1; ++shift; }
      bits |= ((113 - shift) << 23) | ((fraction & 1023) << 13);
    }
  }
  float result;
  memcpy(&result, &bits, sizeof result);
  return result;
}

static uint32_t narrow_overflow(uint32_t sign, unsigned fraction_bits, int mode) {
  unsigned infinity = ((1u << (15 - fraction_bits)) - 1) << fraction_bits;
  bool finite = mode == FE_TOWARDZERO ||
                (mode == FE_UPWARD && sign) ||
                (mode == FE_DOWNWARD && !sign);
  feraiseexcept(FE_OVERFLOW | FE_INEXACT);
  return sign | (finite ? infinity - 1 : infinity);
}

uint32_t __edg_qbe_narrow_from_double(double value, uint32_t bfloat) {
  uint64_t bits;
  memcpy(&bits, &value, sizeof bits);
  uint32_t sign = (uint32_t)(bits >> 48) & 32768;
  unsigned fraction_bits = bfloat ? 7 : 10;
  unsigned bias = bfloat ? 127 : 15;
  unsigned limit = 2 * bias + 1;
  unsigned source_exponent = (unsigned)(bits >> 52) & 2047;
  uint64_t fraction = bits & UINT64_C(0x000fffffffffffff);
  if (source_exponent == 2047) {
    unsigned payload = (unsigned)(fraction >> (52 - fraction_bits));
    if (fraction) {
      if (!(fraction & UINT64_C(0x0008000000000000))) feraiseexcept(FE_INVALID);
      payload |= 1u << (fraction_bits - 1);
    }
    return sign | (limit << fraction_bits) | payload;
  }
  if (!(source_exponent | fraction)) return sign;
  int exponent = (int)source_exponent - 1023 + (int)bias;
  uint64_t mantissa = fraction;
  if (source_exponent) mantissa |= UINT64_C(0x0010000000000000);
  else exponent = 1 - 1023 + (int)bias;
  int mode = fegetround();
  if (exponent >= (int)limit)
    return narrow_overflow(sign, fraction_bits, mode);
  unsigned shift = 52 - fraction_bits;
  if (exponent <= 0) shift += 1 - exponent;
  uint64_t retained = 0, discarded = mantissa, midpoint = UINT64_MAX;
  if (shift < 64) {
    retained = mantissa >> shift;
    discarded = mantissa & ((UINT64_C(1) << shift) - 1);
    midpoint = UINT64_C(1) << (shift - 1);
  }
  bool increment = discarded &&
    (mode == FE_UPWARD ? !sign : mode == FE_DOWNWARD ? sign != 0 :
     mode == FE_TOWARDZERO ? false :
     discarded > midpoint || (discarded == midpoint && (retained & 1)));
  retained += increment;
  unsigned encoded;
  if (exponent <= 0) encoded = (unsigned)retained;
  else {
    if (retained == (UINT64_C(1) << (fraction_bits + 1))) {
      retained >>= 1;
      ++exponent;
    }
    if (exponent >= (int)limit)
      return narrow_overflow(sign, fraction_bits, mode);
    encoded = ((unsigned)exponent << fraction_bits) |
              ((unsigned)retained & ((1u << fraction_bits) - 1));
  }
  if (discarded)
    feraiseexcept(FE_INEXACT | (encoded < (1u << fraction_bits) ? FE_UNDERFLOW : 0));
  return sign | encoded;
}

/* Keep the buffer ABI in native byte order without type-punning or requiring
 * alignment.  Read all inputs before writing an output that may alias them. */
static limbs128 read_limbs(const void *p) {
  limbs128 v;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  memcpy(&v.lo, p, 8);
  memcpy(&v.hi, (const char *)p + 8, 8);
#else
  memcpy(&v.hi, p, 8);
  memcpy(&v.lo, (const char *)p + 8, 8);
#endif
  return v;
}
static void write_limbs(void *p, limbs128 v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  memcpy(p, &v.lo, 8);
  memcpy((char *)p + 8, &v.hi, 8);
#else
  memcpy(p, &v.hi, 8);
  memcpy((char *)p + 8, &v.lo, 8);
#endif
}
static limbs128 negate_limbs(limbs128 a) {
  limbs128 r = {0 - a.lo, ~a.hi + (a.lo == 0)};
  return r;
}
static int compare_limbs(limbs128 a, limbs128 b) {
  if (a.hi != b.hi) return a.hi > b.hi ? 1 : -1;
  return a.lo == b.lo ? 0 : a.lo > b.lo ? 1 : -1;
}
static uint64_t low_mask(unsigned bits) {
  return bits == 64 ? UINT64_MAX : (UINT64_C(1) << bits) - 1;
}
static limbs128 mask_limbs(limbs128 a, unsigned bits) {
  if (bits <= 64) { a.lo &= low_mask(bits); a.hi = 0; }
  else a.hi &= low_mask(bits - 64);
  return a;
}

/* Four 32x32 products, with every intermediate bounded to 64 bits. */
static limbs128 multiply64(uint64_t a, uint64_t b) {
  uint64_t al = (uint32_t)a, ah = a >> 32;
  uint64_t bl = (uint32_t)b, bh = b >> 32;
  uint64_t p = al * bl;
  uint64_t t = ah * bl + (p >> 32);
  uint64_t middle = al * bh + (uint32_t)t;
  limbs128 r = {(middle << 32) | (uint32_t)p,
                ah * bh + (t >> 32) + (middle >> 32)};
  return r;
}

#define MULTIPLY(BITS, TYPE)                                                \
  uint64_t __edg_qbe_umul##BITS(uint64_t a, uint64_t b, void *output) {       \
    limbs128 full = multiply64((TYPE)a, (TYPE)b);                            \
    TYPE product = (TYPE)full.lo;                                           \
    bool overflow = full.hi != 0 || full.lo > low_mask(BITS);                \
    memcpy(output, &product, sizeof product);                               \
    memcpy((char *)output + sizeof product, &overflow, sizeof overflow);     \
    return product;                                                        \
  }
MULTIPLY(8, uint8_t)
MULTIPLY(16, uint16_t)
MULTIPLY(32, uint32_t)
MULTIPLY(64, uint64_t)

static uint64_t population(uint64_t value) {
  value -= (value >> 1) & UINT64_C(0x5555555555555555);
  value = (value & UINT64_C(0x3333333333333333)) +
          ((value >> 2) & UINT64_C(0x3333333333333333));
  value = (value + (value >> 4)) & UINT64_C(0x0f0f0f0f0f0f0f0f);
  return (value * UINT64_C(0x0101010101010101)) >> 56;
}
static uint64_t leading(uint64_t value, unsigned bits) {
  uint64_t count = 0;
  uint64_t mask = UINT64_C(1) << (bits - 1);
  while (mask && !(value & mask)) { ++count; mask >>= 1; }
  return count;
}
static uint64_t trailing(uint64_t value, unsigned bits) {
  uint64_t count = 0;
  if (!value) return bits;
  while (!(value & 1)) { ++count; value >>= 1; }
  return count;
}
#define COUNTS_SMALL(BITS, TYPE)                                            \
  uint32_t __edg_qbe_ctpop##BITS(uint32_t value) {                            \
    return (uint32_t)population((TYPE)value);                                \
  }                                                                        \
  uint32_t __edg_qbe_ctlz##BITS(uint32_t value) {                             \
    return (uint32_t)leading((TYPE)value, BITS);                             \
  }                                                                        \
  uint32_t __edg_qbe_cttz##BITS(uint32_t value) {                             \
    return (uint32_t)trailing((TYPE)value, BITS);                            \
  }
COUNTS_SMALL(8, uint8_t)
COUNTS_SMALL(16, uint16_t)
COUNTS_SMALL(32, uint32_t)
uint64_t __edg_qbe_ctpop64(uint64_t value) { return population(value); }
uint64_t __edg_qbe_ctlz64(uint64_t value) { return leading(value, 64); }
uint64_t __edg_qbe_cttz64(uint64_t value) { return trailing(value, 64); }
uint32_t __edg_qbe_bswap16(uint32_t value) {
  return ((value & 255) << 8) | ((value >> 8) & 255);
}
uint32_t __edg_qbe_bswap32(uint32_t value) {
  value = ((value & UINT32_C(0x00ff00ff)) << 8) |
          ((value >> 8) & UINT32_C(0x00ff00ff));
  return (value << 16) | (value >> 16);
}
uint64_t __edg_qbe_bswap64(uint64_t value) {
  return ((uint64_t)__edg_qbe_bswap32((uint32_t)value) << 32) |
         __edg_qbe_bswap32((uint32_t)(value >> 32));
}

void __edg_qbe_i128_add(void *out, const void *a, const void *b) {
  limbs128 av = read_limbs(a), bv = read_limbs(b);
  limbs128 r = {av.lo + bv.lo, av.hi + bv.hi};
  r.hi += r.lo < av.lo;
  write_limbs(out, r);
}
void __edg_qbe_i128_sub(void *out, const void *a, const void *b) {
  limbs128 av = read_limbs(a), bv = read_limbs(b);
  limbs128 r = {av.lo - bv.lo, av.hi - bv.hi - (av.lo < bv.lo)};
  write_limbs(out, r);
}
void __edg_qbe_i128_mul(void *out, const void *a, const void *b) {
  limbs128 av = read_limbs(a), bv = read_limbs(b);
  limbs128 r = multiply64(av.lo, bv.lo);
  r.hi += av.lo * bv.hi + av.hi * bv.lo;
  write_limbs(out, r);
}
#define I128_BITWISE(NAME, OP)                                              \
  void __edg_qbe_i128_##NAME(void *out, const void *a, const void *b) {       \
    limbs128 av = read_limbs(a), bv = read_limbs(b);                         \
    limbs128 r = {av.lo OP bv.lo, av.hi OP bv.hi};                           \
    write_limbs(out, r);                                                    \
  }
I128_BITWISE(and, &)
I128_BITWISE(or, |)
I128_BITWISE(xor, ^)

/* GCC 15 libgcc2.h (64-bit word configuration) and soft-fp/{quad.h,*.c}
 * define these native TI/TF ABI entry points.  No wide arithmetic or casts
 * are performed here: the system library supplies their implementations. */
extern i128 __divti3(i128, i128);
extern i128 __modti3(i128, i128);
extern u128 __udivti3(u128, u128);
extern u128 __umodti3(u128, u128);
static u128 read_u128(const void *p) { u128 v; memcpy(&v, p, 16); return v; }
static i128 read_i128(const void *p) { i128 v; memcpy(&v, p, 16); return v; }
static void write_u128(void *p, u128 v) { memcpy(p, &v, 16); }
static void write_i128(void *p, i128 v) { memcpy(p, &v, 16); }
#define I128_LIBCALL(NAME, READ, WRITE, FN)                                  \
  void __edg_qbe_i128_##NAME(void *out, const void *a, const void *b) {       \
    WRITE(out, FN(READ(a), READ(b)));                                        \
  }
I128_LIBCALL(udiv, read_u128, write_u128, __udivti3)
I128_LIBCALL(urem, read_u128, write_u128, __umodti3)
I128_LIBCALL(div, read_i128, write_i128, __divti3)
I128_LIBCALL(rem, read_i128, write_i128, __modti3)

void __edg_qbe_i128_shl(void *out, const void *a, uint32_t shift) {
  limbs128 av = read_limbs(a), r = {0, 0};
  if (!shift) r = av;
  else if (shift < 64) {
    r.lo = av.lo << shift;
    r.hi = (av.hi << shift) | (av.lo >> (64 - shift));
  } else if (shift < 128) r.hi = av.lo << (shift - 64);
  write_limbs(out, r);
}
static limbs128 shift_right(limbs128 av, uint32_t shift, uint64_t fill) {
  limbs128 r = {fill, fill};
  if (!shift) r = av;
  else if (shift < 64) {
    r.lo = (av.lo >> shift) | (av.hi << (64 - shift));
    r.hi = (av.hi >> shift) | (fill << (64 - shift));
  } else if (shift < 128) {
    r.lo = av.hi >> (shift - 64);
    if (shift != 64) r.lo |= fill << (128 - shift);
  }
  return r;
}
void __edg_qbe_i128_shr(void *out, const void *a, uint32_t shift) {
  write_limbs(out, shift_right(read_limbs(a), shift, 0));
}
void __edg_qbe_i128_sar(void *out, const void *a, uint32_t shift) {
  limbs128 av = read_limbs(a);
  write_limbs(out, shift_right(av, shift, 0 - (av.hi >> 63)));
}
void __edg_qbe_i128_neg(void *out, const void *a) {
  write_limbs(out, negate_limbs(read_limbs(a)));
}
static int signed_compare(const void *a, const void *b) {
  limbs128 av = read_limbs(a), bv = read_limbs(b);
  av.hi ^= UINT64_C(1) << 63;
  bv.hi ^= UINT64_C(1) << 63;
  return compare_limbs(av, bv);
}
static int unsigned_compare(const void *a, const void *b) {
  return compare_limbs(read_limbs(a), read_limbs(b));
}
#define I128_COMPARE(NAME, CMP, OP)                                         \
  int __edg_qbe_i128_##NAME(const void *a, const void *b) {                   \
    return CMP(a, b) OP 0;                                                  \
  }
I128_COMPARE(eq, unsigned_compare, ==)
I128_COMPARE(ne, unsigned_compare, !=)
I128_COMPARE(lt, signed_compare, <)
I128_COMPARE(le, signed_compare, <=)
I128_COMPARE(gt, signed_compare, >)
I128_COMPARE(ge, signed_compare, >=)
I128_COMPARE(ult, unsigned_compare, <)
I128_COMPARE(ule, unsigned_compare, <=)
I128_COMPARE(ugt, unsigned_compare, >)
I128_COMPARE(uge, unsigned_compare, >=)

extern long double __addtf3(long double, long double);
extern long double __subtf3(long double, long double);
extern long double __multf3(long double, long double);
extern long double __divtf3(long double, long double);
/* AArch64's libgcc comparison return mode is SI (32-bit signed integer).
 * letf2 returns +2 for unordered; getf2 returns -2 for unordered. */
extern int __eqtf2(long double, long double);
extern int __letf2(long double, long double);
extern int __getf2(long double, long double);
extern long double __floatditf(int64_t);
extern long double __floatunditf(uint64_t);
extern long double __extendsftf2(float);
extern long double __extenddftf2(double);
extern int64_t __fixtfdi(long double);
extern uint64_t __fixunstfdi(long double);
extern float __trunctfsf2(long double);
extern double __trunctfdf2(long double);
extern long double __floattitf(i128);
extern long double __floatuntitf(u128);
extern i128 __fixtfti(long double);
extern u128 __fixunstfti(long double);
extern double __floattidf(i128);
extern double __floatuntidf(u128);
extern i128 __fixdfti(double);
extern u128 __fixunsdfti(double);
extern float __floattisf(i128);
extern float __floatuntisf(u128);
extern i128 __fixsfti(float);
extern u128 __fixunssfti(float);
static long double read_tf(const void *p) {
  long double v; memcpy(&v, p, 16); return v;
}
static void write_tf(void *p, long double v) { memcpy(p, &v, 16); }
#define TF_BINARY(NAME, FN)                                                \
  void __edg_qbe_tf_##NAME(void *out, const void *a, const void *b) {         \
    write_tf(out, FN(read_tf(a), read_tf(b)));                               \
  }
TF_BINARY(add, __addtf3)
TF_BINARY(sub, __subtf3)
TF_BINARY(mul, __multf3)
TF_BINARY(div, __divtf3)
void __edg_qbe_tf_neg(void *out, const void *a) {
  limbs128 v = read_limbs(a);
  v.hi ^= UINT64_C(1) << 63;
  write_limbs(out, v);
}
#define TF_COMPARE(NAME, FN, OP)                                           \
  int __edg_qbe_tf_##NAME(const void *a, const void *b) {                    \
    return FN(read_tf(a), read_tf(b)) OP 0;                                 \
  }
TF_COMPARE(eq, __eqtf2, ==)
TF_COMPARE(ne, __eqtf2, !=)
TF_COMPARE(lt, __letf2, <)
TF_COMPARE(le, __letf2, <=)
TF_COMPARE(gt, __getf2, >)
TF_COMPARE(ge, __getf2, >=)

void __edg_qbe_i128_from_signed(void *out, int64_t a) {
  limbs128 v = {(uint64_t)a, 0 - ((uint64_t)a >> 63)};
  write_limbs(out, v);
}
void __edg_qbe_i128_from_unsigned(void *out, uint64_t a) {
  limbs128 v = {a, 0};
  write_limbs(out, v);
}
void __edg_qbe_tf_from_signed(void *out, int64_t a) { write_tf(out, __floatditf(a)); }
void __edg_qbe_tf_from_unsigned(void *out, uint64_t a) { write_tf(out, __floatunditf(a)); }
void __edg_qbe_tf_from_double(void *out, double a) { write_tf(out, __extenddftf2(a)); }
int64_t __edg_qbe_tf_to_signed(const void *a) { return __fixtfdi(read_tf(a)); }
uint64_t __edg_qbe_tf_to_unsigned(const void *a) { return __fixunstfdi(read_tf(a)); }
double __edg_qbe_tf_to_double(const void *a) { return __trunctfdf2(read_tf(a)); }
void __edg_qbe_tf_from_i128(void *out, const void *a) {
  write_tf(out, __floattitf(read_i128(a)));
}
void __edg_qbe_tf_from_u128(void *out, const void *a) {
  write_tf(out, __floatuntitf(read_u128(a)));
}
void __edg_qbe_i128_from_tf(void *out, const void *a) {
  write_i128(out, __fixtfti(read_tf(a)));
}
void __edg_qbe_u128_from_tf(void *out, const void *a) {
  write_u128(out, __fixunstfti(read_tf(a)));
}
double __edg_qbe_i128_to_double(const void *a, int is_unsigned) {
  return is_unsigned ? __floatuntidf(read_u128(a)) : __floattidf(read_i128(a));
}
void __edg_qbe_i128_from_double(void *out, double a, int is_unsigned) {
  if (is_unsigned) write_u128(out, __fixunsdfti(a));
  else write_i128(out, __fixdfti(a));
}
float __edg_qbe_i128_to_float(const void *a, int is_unsigned) {
  return is_unsigned ? __floatuntisf(read_u128(a)) : __floattisf(read_i128(a));
}
void __edg_qbe_i128_from_float(void *out, float a, int is_unsigned) {
  if (is_unsigned) write_u128(out, __fixunssfti(a));
  else write_i128(out, __fixsfti(a));
}
void __edg_qbe_tf_from_float(void *out, float a) { write_tf(out, __extendsftf2(a)); }
float __edg_qbe_tf_to_float(const void *a) { return __trunctfsf2(read_tf(a)); }

#define SIGNED_MULTIPLY(BITS, TYPE)                                         \
  int __edg_qbe_smul##BITS(int64_t a, int64_t b, void *output) {             \
    uint64_t av = (TYPE)a, bv = (TYPE)b;                                    \
    int aneg = (int)((av >> (BITS - 1)) & 1);                               \
    int bneg = (int)((bv >> (BITS - 1)) & 1);                               \
    uint64_t am = (aneg ? 0 - av : av) & low_mask(BITS);                     \
    uint64_t bm = (bneg ? 0 - bv : bv) & low_mask(BITS);                     \
    limbs128 full = multiply64(am, bm);                                    \
    uint64_t limit = UINT64_C(1) << (BITS - 1);                             \
    bool overflow = full.hi != 0 ||                                        \
      (aneg != bneg ? full.lo > limit : full.lo >= limit);                  \
    TYPE product = (TYPE)(aneg != bneg ? 0 - full.lo : full.lo);             \
    memcpy(output, &product, sizeof product);                              \
    memcpy((char *)output + sizeof product, &overflow, sizeof overflow);    \
    return overflow;                                                       \
  }
SIGNED_MULTIPLY(8, uint8_t)
SIGNED_MULTIPLY(16, uint16_t)
SIGNED_MULTIPLY(32, uint32_t)
SIGNED_MULTIPLY(64, uint64_t)

typedef struct { uint64_t limb[4]; int negative; } checked_integer;
static checked_integer checked_read(const void *p, unsigned bits, int uns) {
  unsigned char bytes[16] = {0};
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  memcpy(bytes, p, (bits + 7) / 8);
#else
  memcpy(bytes + 16 - (bits + 7) / 8, p, (bits + 7) / 8);
#endif
  limbs128 value = mask_limbs(read_limbs(bytes), bits);
  int negative = !uns && (bits <= 64 ?
    ((value.lo >> (bits - 1)) & 1) : ((value.hi >> (bits - 65)) & 1));
  if (negative) value = mask_limbs(negate_limbs(value), bits);
  checked_integer result = {{value.lo, value.hi, 0, 0}, negative};
  return result;
}
static int checked_compare(checked_integer a, checked_integer b) {
  for (int i = 3; i >= 0; --i) {
    if (a.limb[i] != b.limb[i]) return a.limb[i] > b.limb[i] ? 1 : -1;
  }
  return 0;
}
static checked_integer checked_add(checked_integer a, checked_integer b) {
  checked_integer result = {{0, 0, 0, 0}, a.negative};
  if (a.negative == b.negative) {
    uint64_t carry = 0;
    for (int i = 0; i != 4; ++i) {
      uint64_t sum = a.limb[i] + b.limb[i];
      uint64_t next = sum + carry;
      carry = (sum < a.limb[i]) | (next < sum);
      result.limb[i] = next;
    }
  } else {
    if (checked_compare(a, b) < 0) {
      checked_integer tmp = a; a = b; b = tmp;
    }
    result.negative = a.negative;
    uint64_t borrow = 0;
    for (int i = 0; i != 4; ++i) {
      uint64_t subtrahend = b.limb[i] + borrow;
      uint64_t next = a.limb[i] - subtrahend;
      borrow = (subtrahend < b.limb[i]) | (a.limb[i] < subtrahend);
      result.limb[i] = next;
    }
  }
  if (!(result.limb[0] | result.limb[1] | result.limb[2] | result.limb[3]))
    result.negative = 0;
  return result;
}
static checked_integer checked_multiply(checked_integer a, checked_integer b) {
  checked_integer result = {{0, 0, 0, 0}, a.negative != b.negative};
  for (int i = 0; i != 2; ++i) {
    uint64_t carry = 0;
    for (int j = 0; j != 2; ++j) {
      limbs128 product = multiply64(a.limb[i], b.limb[j]);
      uint64_t sum = product.lo + result.limb[i + j];
      product.hi += sum < product.lo;
      uint64_t next = sum + carry;
      product.hi += next < sum;
      result.limb[i + j] = next;
      carry = product.hi;
    }
    result.limb[i + 2] = carry;
  }
  if (!(result.limb[0] | result.limb[1] | result.limb[2] | result.limb[3]))
    result.negative = 0;
  return result;
}

/* GCC overflow builtins calculate in an infinite-precision signed domain
 * before conversion to the destination type, even with mixed operand types. */
int __edg_qbe_overflow(void *out, uint32_t outbits, int outunsigned,
                       const void *a, uint32_t abits, int aunsigned,
                       const void *b, uint32_t bbits, int bunsigned,
                       uint32_t op) {
  checked_integer av = checked_read(a, abits, aunsigned);
  checked_integer bv = checked_read(b, bbits, bunsigned);
  checked_integer value;
  if (op == 2) value = checked_multiply(av, bv);
  else {
    if (op == 1) bv.negative = !bv.negative;
    value = checked_add(av, bv);
  }
  limbs128 low = {value.limb[0], value.limb[1]};
  limbs128 converted = value.negative ? negate_limbs(low) : low;
  unsigned char bytes[16];
  write_limbs(bytes, converted);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  memcpy(out, bytes, (outbits + 7) / 8);
#else
  memcpy(out, bytes + 16 - (outbits + 7) / 8, (outbits + 7) / 8);
#endif
  if (value.limb[2] || value.limb[3]) return 1;
  if (outunsigned) {
    if (value.negative) return 1;
    return compare_limbs(low, mask_limbs(low, outbits)) != 0;
  }
  limbs128 limit = {0, 0};
  if (outbits <= 64) limit.lo = UINT64_C(1) << (outbits - 1);
  else limit.hi = UINT64_C(1) << (outbits - 65);
  int cmp = compare_limbs(low, limit);
  return value.negative ? cmp > 0 : cmp >= 0;
}

static unsigned tf_exponent(limbs128 a) { return (unsigned)((a.hi >> 48) & 32767); }
static int tf_fraction(limbs128 a) {
  return (a.lo | (a.hi & UINT64_C(0x0000ffffffffffff))) != 0;
}
int __edg_qbe_tf_isnan(const void *a) {
  limbs128 v = read_limbs(a);
  return tf_exponent(v) == 32767 && tf_fraction(v);
}
int __edg_qbe_tf_isinf(const void *a) {
  limbs128 v = read_limbs(a);
  return tf_exponent(v) == 32767 && !tf_fraction(v);
}
int __edg_qbe_tf_isinf_sign(const void *a) {
  limbs128 v = read_limbs(a);
  if (tf_exponent(v) != 32767 || tf_fraction(v)) return 0;
  return v.hi >> 63 ? -1 : 1;
}
int __edg_qbe_tf_isfinite(const void *a) {
  return tf_exponent(read_limbs(a)) != 32767;
}
int __edg_qbe_tf_isnormal(const void *a) {
  unsigned exponent = tf_exponent(read_limbs(a));
  return exponent != 0 && exponent != 32767;
}
int __edg_qbe_tf_signbit(const void *a) { return (int)(read_limbs(a).hi >> 63); }
int __edg_qbe_tf_isunordered(const void *a, const void *b) {
  return __edg_qbe_tf_isnan(a) || __edg_qbe_tf_isnan(b);
}
