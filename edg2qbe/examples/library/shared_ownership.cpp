#include <cstdint>
#include <cstdio>
#include <memory>

struct State {
  int live = 0;
  int destroyed = 0;
};

struct alignas(128) Node : std::enable_shared_from_this<Node> {
  State* state;
  int value;

  Node(int value, State& state) : state(&state), value(value) { ++state.live; }
  Node(const Node&) = delete;
  Node& operator=(const Node&) = delete;
  ~Node() {
    --state->live;
    ++state->destroyed;
  }
};

bool aligned(const Node* pointer) {
  return reinterpret_cast<std::uintptr_t>(pointer) % alignof(Node) == 0;
}

int main() {
  State state;
  std::weak_ptr<Node> observer;
  std::shared_ptr<int> alias;
  int retained_value = 0;
  {
    auto owner = std::make_shared<Node>(21, state);
    if (state.live != 1 || !aligned(owner.get()))
      return 1;
    observer = owner->weak_from_this();
    auto self = owner->shared_from_this();
    if (observer.expired() || self.get() != owner.get() || owner.use_count() != 2)
      return 2;
    alias = std::shared_ptr<int>(owner, &owner->value);
    if (alias.get() != &owner->value || owner.use_count() != 3 ||
        owner.owner_before(alias) || alias.owner_before(owner))
      return 3;
    {
      auto locked = observer.lock();
      if (locked.get() != owner.get() || locked.use_count() != 4)
        return 4;
      *alias = 42;
      if (locked->value != 42 || self->value != 42)
        return 5;
    }
    self.reset();
    owner.reset();
    if (state.live != 1 || state.destroyed != 0 || observer.expired() ||
        alias.use_count() != 1 || *alias != 42)
      return 6;
    {
      auto retained = observer.lock();
      if (!retained || retained->value != 42 || !aligned(retained.get()))
        return 7;
      retained_value = retained->value;
    }
  }
  if (state.live != 1 || observer.expired() || *alias != 42)
    return 8;
  alias.reset();
  if (state.live != 0 || state.destroyed != 1 || !observer.expired() || observer.lock())
    return 9;

  int unowned_exception = 0;
  {
    Node unowned(5, state);
    if (!aligned(&unowned) || !unowned.weak_from_this().expired())
      return 10;
    try {
      (void)unowned.shared_from_this();
      return 11;
    } catch (const std::bad_weak_ptr&) {
      unowned_exception = 1;
    }
  }
  if (state.live != 0 || state.destroyed != 2 || unowned_exception != 1)
    return 12;

  {
    auto allocated = std::allocate_shared<Node>(std::allocator<Node>{}, 7, state);
    auto unique = std::make_unique<Node>(8, state);
    if (state.live != 2 || !aligned(allocated.get()) || !aligned(unique.get()) ||
        allocated->value != 7 || unique->value != 8)
      return 13;
    auto allocated_self = allocated->shared_from_this();
    if (allocated_self.get() != allocated.get())
      return 14;
    allocated.reset();
    if (state.live != 2 || allocated_self->value != 7)
      return 15;
    allocated_self.reset();
    if (state.live != 1 || state.destroyed != 3)
      return 16;
    unique.reset();
  }
  if (state.live != 0 || state.destroyed != 4 || !observer.expired())
    return 17;
  std::printf("shared=%d expired=%d aligned=1 destroyed=%d unowned=%d\n",
              retained_value, static_cast<int>(observer.expired()), state.destroyed,
              unowned_exception);
}
