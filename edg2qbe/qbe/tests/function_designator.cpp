int plus_one(int n) { return n + 1; }

int main() {
  int (*volatile callback)(int) = plus_one;
  return (*callback)(41) == 42 ? 0 : 1;
}
