#include <atomic>
#include <cstdio>
#include <cstring>
#include <future>
#include <stdexcept>

struct Guard {
  static std::atomic<int> live, destroyed;
  Guard() { ++live; }
  ~Guard() { --live; ++destroyed; }
};
std::atomic<int> Guard::live{0}, Guard::destroyed{0};

__attribute__((noinline)) static void leaf(int kind) {
  Guard guard;
  if (kind == 1) throw 37;
  if (kind == 2) throw std::runtime_error("foreign");
  throw 1.5;
}

__attribute__((noinline)) static void middle(int kind) {
  Guard guard;
  try { leaf(kind); }
  catch (const std::logic_error &) { std::puts("incorrect catch"); }
}

__attribute__((noinline)) static void rethrow() {
  Guard guard;
  try { leaf(1); }
  catch (int) { throw; }
}

int main() {
  int score = 0;
  for (int kind = 1; kind <= 3; ++kind) {
    try { middle(kind); }
    catch (int value) { score += value == 37 ? 1 : -100; }
    catch (const std::runtime_error &error) {
      score += std::strcmp(error.what(), "foreign") == 0 ? 2 : -100;
    }
    catch (...) { score += 4; }
  }
  try { rethrow(); }
  catch (int value) { score += value == 37 ? 8 : -100; }

  auto future = std::async(std::launch::async, []() -> int {
    Guard guard;
    throw std::runtime_error("worker");
  });
  try { (void)future.get(); }
  catch (const std::exception &error) {
    score += std::strcmp(error.what(), "worker") == 0 ? 16 : -100;
  }
  int live = Guard::live.load(), destroyed = Guard::destroyed.load();
  std::printf("score=%d live=%d destroyed=%d\n", score, live, destroyed);
  return score == 31 && live == 0 && destroyed == 9 ? 0 : 1;
}
