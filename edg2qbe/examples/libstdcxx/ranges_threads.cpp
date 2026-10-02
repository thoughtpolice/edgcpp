#include <atomic>
#include <future>
#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

int main() {
  std::vector<int> values{1, 2, 3, 4, 5, 6};
  auto squares = values
      | std::views::filter([](int value) { return value % 2 == 0; })
      | std::views::transform([](int value) { return value * value; });
  int sum = 0;
  for (int value : squares) {
    sum += value;
  }

  std::atomic<int> count{0};
  std::thread worker([&count] { count.fetch_add(7); });
  auto result = std::async(std::launch::async, [sum] { return sum + 1; });
  worker.join();
  const int answer = result.get();
  std::cout << "squares=" << sum << " async=" << answer
            << " thread=" << count.load() << '\n';
  return sum == 56 && answer == 57 && count.load() == 7 ? 0 : 1;
}
