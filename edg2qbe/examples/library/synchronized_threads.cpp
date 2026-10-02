#include <atomic>
#include <barrier>
#include <cstdio>
#include <latch>
#include <semaphore>
#include <stop_token>
#include <thread>

int main() {
  std::latch ready(2);
  int phases = 0;
  std::barrier phase(3, [&]() noexcept { ++phases; });
  std::counting_semaphore<2> gate(0);
  std::atomic<int> publication{0};
  std::atomic<int> callbacks{0};
  int inputs[2] = {};
  int results[2] = {};
  int observed[2] = {};
  int payload = 0;

  const auto worker = [&](std::stop_token token, int index) {
    std::stop_callback on_stop(token, [&]() noexcept {
      callbacks.fetch_add(1, std::memory_order_relaxed);
    });
    inputs[index] = index + 10;
    ready.count_down();
    phase.arrive_and_wait();
    results[index] = inputs[1 - index] + index + 1;
    phase.arrive_and_wait();
    gate.acquire();
    publication.wait(0, std::memory_order_acquire);
    observed[index] = payload + (token.stop_requested() ? 1 : 0);
  };

  std::jthread first(worker, 0);
  std::jthread second(worker, 1);
  ready.wait();
  phase.arrive_and_wait();
  phase.arrive_and_wait();
  const int phase_sum = results[0] + results[1];
  const bool first_stop = first.request_stop();
  const bool second_stop = second.request_stop();
  const bool repeated_first = first.request_stop();
  const bool repeated_second = second.request_stop();

  gate.release(2);
  payload = 77;
  publication.store(1, std::memory_order_release);
  publication.notify_all();
  first.join();
  second.join();

  if (inputs[0] != 10 || inputs[1] != 11 || results[0] != 12 || results[1] != 12 ||
      phase_sum != 24 || phases != 2)
    return 1;
  if (!first_stop || !second_stop || repeated_first || repeated_second ||
      callbacks.load(std::memory_order_relaxed) != 2)
    return 2;
  if (observed[0] != 78 || observed[1] != 78 || publication.load() != 1 ||
      first.joinable() || second.joinable())
    return 3;
  std::printf("threads=%d phases=%d callbacks=%d published=%d stopped=1\n",
              phase_sum, phases, callbacks.load(std::memory_order_relaxed),
              observed[0] + observed[1]);
}
