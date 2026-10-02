#include <cstdarg>
#include <cstdio>
#include <cmath>

struct PairTF { long double a, b; };
struct FourFloat { float a, b, c, d; };
struct FourHalf { _Float16 a, b, c, d; };
struct Big { unsigned long a, b, c, d, e; };
extern "C" long double gcc_tf(long double, long double);
extern "C" unsigned __int128 gcc_i128(unsigned __int128, unsigned __int128);
extern "C" PairTF gcc_pair(PairTF);
extern "C" FourFloat gcc_hfa(FourFloat);
extern "C" Big gcc_big(Big);
extern "C" _Float16 gcc_half(_Float16);
extern "C" FourHalf gcc_half_hfa(FourHalf);
extern "C" FourHalf gcc_half_stack(_Float16, _Float16, _Float16, _Float16,
                                  _Float16, _Float16, _Float16, FourHalf, _Float16);
extern "C" int gcc_calls_qbe();

extern "C" long double qbe_tf(long double a, long double b) { return a * 3 + b; }
extern "C" unsigned __int128 qbe_i128(unsigned __int128 a, unsigned __int128 b) {
  return (a << 3) ^ b;
}
extern "C" PairTF qbe_pair(PairTF p) { return {p.a + 1, p.b * 2}; }
extern "C" FourFloat qbe_hfa(FourFloat p) { return {p.d, p.c, p.b, p.a}; }
extern "C" Big qbe_big(Big p) { return {p.e, p.d, p.c, p.b, p.a}; }
extern "C" _Float16 qbe_half(_Float16 x) { return -x + (_Float16)2; }
extern "C" FourHalf qbe_half_hfa(FourHalf p) { return {p.d, p.c + (_Float16)1, p.b, p.a}; }
extern "C" FourHalf qbe_half_stack(_Float16 a, _Float16 b, _Float16 c, _Float16 d,
                                  _Float16 e, _Float16 f, _Float16 g, FourHalf p, _Float16 tail) {
  p.a += a + b + c + d + e + f + g;
  p.d += tail;
  return p;
}
extern "C" _Float16 qbe_half_from_double(double x) { return (_Float16)x; }
extern "C" _Float16 qbe_half_from_tf(long double x) { return (_Float16)x; }
extern "C" long double qbe_var_tf(int count, ...) {
  va_list args;
  va_start(args, count);
  long double sum = 0;
  for (int i = 0; i < count; ++i) sum += va_arg(args, long double);
  va_end(args);
  return sum;
}
extern "C" unsigned __int128 qbe_var_i128(int count, ...) {
  va_list args;
  va_start(args, count);
  unsigned __int128 sum = 0;
  for (int i = 0; i < count; ++i) sum += va_arg(args, unsigned __int128);
  va_end(args);
  return sum;
}

int main() {
  const long double a = 1 + 0x1p-100L;
  const unsigned __int128 wide = (static_cast<unsigned __int128>(1) << 100) + 17;
  if (gcc_tf(a, 4) != a * 5 + 4) return 1;
  if (gcc_i128(wide, 91) != ((wide << 2) + 91)) return 2;
  PairTF p = gcc_pair({a, 7});
  if (p.a != a * 2 || p.b != 10) return 3;
  FourFloat h = gcc_hfa({1, 2, 3, 4});
  if (h.a != 2 || h.b != 3 || h.c != 4 || h.d != 5) return 4;
  Big b = gcc_big({1, 2, 3, 4, 5});
  if (b.a != 2 || b.b != 4 || b.c != 6 || b.d != 8 || b.e != 10) return 5;
  if (qbe_var_tf(12, 1.L, 2.L, 3.L, 4.L, 5.L, 6.L, 7.L, 8.L, 9.L, 10.L, 11.L, 12.L) != 78.L) return 6;
  if (qbe_var_i128(5, wide, wide, wide, wide, wide) != wide * 5) return 7;
  if (sqrtl(gcc_tf(2, 6)) != 4 || fabsl(gcc_tf(-1, 0)) != 5) return 8;
  int value = 7;
  try { throw &value; }
  catch (const int *pointer) { if (pointer != &value || *pointer != 7) return 9; }
  int reverse = gcc_calls_qbe();
  if (reverse) return 10 + reverse;
  if ((float)gcc_half((_Float16)0.5) != 1.5f) return 20;
  FourHalf half = gcc_half_hfa({1, 2, 3, 4});
  if ((float)half.a != 4 || (float)half.b != 4 || (float)half.c != 2 || (float)half.d != 1) return 21;
  half = gcc_half_stack(1, 2, 3, 4, 5, 6, 7, {1, 2, 3, 4}, 8);
  if ((float)half.a != 29 || (float)half.b != 2 || (float)half.c != 3 || (float)half.d != 12) return 22;
  std::puts("native ABI: tf128 i128 HFA aggregates varargs pointer-catch OK");
}
