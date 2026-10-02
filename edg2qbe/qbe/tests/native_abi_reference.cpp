struct PairTF { long double a, b; };
struct FourFloat { float a, b, c, d; };
struct FourHalf { _Float16 a, b, c, d; };
struct Big { unsigned long a, b, c, d, e; };
extern "C" long double qbe_tf(long double, long double);
extern "C" unsigned __int128 qbe_i128(unsigned __int128, unsigned __int128);
extern "C" PairTF qbe_pair(PairTF);
extern "C" FourFloat qbe_hfa(FourFloat);
extern "C" Big qbe_big(Big);
extern "C" _Float16 qbe_half(_Float16);
extern "C" FourHalf qbe_half_hfa(FourHalf);
extern "C" FourHalf qbe_half_stack(_Float16, _Float16, _Float16, _Float16,
                                  _Float16, _Float16, _Float16, FourHalf, _Float16);
extern "C" _Float16 qbe_half_from_double(double);
extern "C" _Float16 qbe_half_from_tf(long double);
extern "C" long double qbe_var_tf(int, ...);
extern "C" unsigned __int128 qbe_var_i128(int, ...);
extern "C" long double gcc_tf(long double a, long double b) { return a * 5 + b; }
extern "C" unsigned __int128 gcc_i128(unsigned __int128 a, unsigned __int128 b) { return (a << 2) + b; }
extern "C" PairTF gcc_pair(PairTF p) { return {p.a * 2, p.b + 3}; }
extern "C" FourFloat gcc_hfa(FourFloat p) { return {p.a + 1, p.b + 1, p.c + 1, p.d + 1}; }
extern "C" Big gcc_big(Big p) { return {p.a * 2, p.b * 2, p.c * 2, p.d * 2, p.e * 2}; }
extern "C" _Float16 gcc_half(_Float16 x) { return -x + (_Float16)2; }
extern "C" FourHalf gcc_half_hfa(FourHalf p) { return {p.d, (_Float16)(p.c + 1), p.b, p.a}; }
extern "C" FourHalf gcc_half_stack(_Float16 a, _Float16 b, _Float16 c, _Float16 d,
                                  _Float16 e, _Float16 f, _Float16 g, FourHalf p, _Float16 tail) {
  p.a += a + b + c + d + e + f + g;
  p.d += tail;
  return p;
}
extern "C" int gcc_calls_qbe() {
  const long double a = 1 + 0x1p-100L;
  const unsigned __int128 wide = (static_cast<unsigned __int128>(1) << 100) + 17;
  if (qbe_tf(a, 4) != a * 3 + 4) return 1;
  if (qbe_i128(wide, 91) != ((wide << 3) ^ 91)) return 2;
  PairTF p = qbe_pair({a, 7});
  if (p.a != a + 1 || p.b != 14) return 3;
  FourFloat h = qbe_hfa({1, 2, 3, 4});
  if (h.a != 4 || h.b != 3 || h.c != 2 || h.d != 1) return 4;
  Big b = qbe_big({1, 2, 3, 4, 5});
  if (b.a != 5 || b.b != 4 || b.c != 3 || b.d != 2 || b.e != 1) return 5;
  if (qbe_var_tf(12, 1.L, 2.L, 3.L, 4.L, 5.L, 6.L, 7.L, 8.L, 9.L, 10.L, 11.L, 12.L) != 78.L) return 6;
  if (qbe_var_i128(5, wide, wide, wide, wide, wide) != wide * 5) return 7;
  if ((float)qbe_half((_Float16)0.5) != 1.5f) return 8;
  FourHalf half = qbe_half_hfa({1, 2, 3, 4});
  if ((float)half.a != 4 || (float)half.b != 4 || (float)half.c != 2 || (float)half.d != 1) return 9;
  half = qbe_half_stack(1, 2, 3, 4, 5, 6, 7, {1, 2, 3, 4}, 8);
  if ((float)half.a != 29 || (float)half.b != 2 || (float)half.c != 3 || (float)half.d != 12) return 10;
  // Values just above a binary16 midpoint must not round through binary32/64.
  if ((float)qbe_half_from_double(0x1.0020000000001p0) != 0x1.004p0f) return 11;
  if ((float)qbe_half_from_tf(1.L + 0x1p-11L + 0x1p-80L) != 0x1.004p0f) return 12;
  return 0;
}
