constexpr long double scale = 1.25L;
static_assert(scale * 4.0L == 5.0L);
static_assert(sizeof(long double) == 16);
static_assert(1.0L + 0x1p-100L > 1.0L);

int sum_squares(int limit) {
  int total = 0;
  for (int i = 0; i < limit; ++i) {
    if (i % 2 == 0) {
      total += i * i;
    }
  }
  return total;
}

int main() {
  return sum_squares(6) == 20 ? 0 : 1;
}
