#include <cstddef>
#include <cstdio>
#include <memory>
#include <vector>

struct State {
  int live = 0;
  int blocks = 0;
  std::size_t slots = 0;
  int copies_before_throw = -1;
  bool throw_constructor = false;
  int rejected = 0;
};

struct CopyFailure {};
struct ConstructionFailure {};

struct Element {
  State* state;
  int value;

  Element(int value, State& state) : state(&state), value(value) {
    if (state.throw_constructor) {
      ++state.rejected;
      throw ConstructionFailure{};
    }
    ++state.live;
  }
  Element(const Element& other) : state(other.state), value(other.value) {
    if (state->copies_before_throw == 0) {
      ++state->rejected;
      throw CopyFailure{};
    }
    if (state->copies_before_throw > 0)
      --state->copies_before_throw;
    ++state->live;
  }
  Element(Element&& other) noexcept(false) : state(other.state), value(other.value) {
    ++state->live;
    other.value = -1;
  }
  Element& operator=(const Element&) = delete;
  Element& operator=(Element&&) = delete;
  ~Element() { --state->live; }
};

template <class T>
struct CountingAllocator {
  using value_type = T;
  State* state;

  explicit CountingAllocator(State& state) noexcept : state(&state) {}
  template <class U>
  CountingAllocator(const CountingAllocator<U>& other) noexcept : state(other.state) {}

  T* allocate(std::size_t count) {
    T* memory = std::allocator<T>{}.allocate(count);
    ++state->blocks;
    state->slots += count;
    return memory;
  }
  void deallocate(T* memory, std::size_t count) noexcept {
    --state->blocks;
    state->slots -= count;
    std::allocator<T>{}.deallocate(memory, count);
  }
  template <class U>
  bool operator==(const CountingAllocator<U>& other) const noexcept {
    return state == other.state;
  }
};

int main() {
  State state;
  int exceptions = 0;
  {
    std::vector<Element, CountingAllocator<Element>> values{CountingAllocator<Element>{state}};
    values.reserve(4);
    const std::size_t original_capacity = values.capacity();
    if (original_capacity < 4)
      return 1;
    for (std::size_t i = 0; i < original_capacity; ++i)
      values.emplace_back(static_cast<int>(i) + 10, state);
    const auto intact = [&] {
      if (values.size() != original_capacity ||
          state.live != static_cast<int>(values.size()) || state.blocks != 1 ||
          state.slots != values.capacity())
        return false;
      for (std::size_t i = 0; i < values.size(); ++i)
        if (values[i].value != static_cast<int>(i) + 10)
          return false;
      return true;
    };
    if (!intact())
      return 2;
    Element* original_data = values.data();

    state.copies_before_throw = 1;
    try {
      values.reserve(original_capacity + 1);
      return 3;
    } catch (const CopyFailure&) {
      ++exceptions;
    }
    state.copies_before_throw = -1;
    if (!intact() || values.capacity() != original_capacity ||
        values.data() != original_data)
      return 4;

    state.throw_constructor = true;
    try {
      values.emplace_back(99, state);
      return 5;
    } catch (const ConstructionFailure&) {
      ++exceptions;
    }
    if (!intact() || values.capacity() != original_capacity ||
        values.data() != original_data)
      return 6;
    state.throw_constructor = false;

    values.reserve(original_capacity + 1);
    if (values.capacity() <= original_capacity || !intact())
      return 7;
    Element* replacement_data = values.data();
    const auto replacement_capacity = values.capacity();
    state.throw_constructor = true;
    try {
      values.emplace_back(99, state);
      return 8;
    } catch (const ConstructionFailure&) {
      ++exceptions;
    }
    state.throw_constructor = false;
    if (!intact() || values.data() != replacement_data ||
        values.capacity() != replacement_capacity)
      return 9;
    values.emplace_back(99, state);
    if (values.back().value != 99 || values.size() != original_capacity + 1 ||
        state.live != static_cast<int>(values.size()) || state.blocks != 1 ||
        state.slots != values.capacity())
      return 10;
    values.clear();
    if (!values.empty() || state.live != 0 || state.blocks != 1 ||
        state.slots != values.capacity())
      return 11;
  }
  if (exceptions != 3 || state.rejected != 3 || state.live != 0 ||
      state.blocks != 0 || state.slots != 0)
    return 12;
  std::printf("vector_preserved=1 exceptions=%d rejected=%d live=%d blocks=%d\n",
              exceptions, state.rejected, state.live, state.blocks);
}
