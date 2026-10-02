template<class T> T tracked(T value) {
  struct Guard {
    T &value;
    explicit Guard(T &v) : value(v) { value += 2; }
    ~Guard() { value *= 3; }
  };
  { Guard guard(value); }
  return value;
}

template<class T> T &shared() { static T value = 0; return value; }

#ifdef QBE_ODR_OTHER_TU
extern "C" int peer(int value) { return tracked(value); }
extern "C" int *peer_counter() { return &shared<int>(); }
#else
extern "C" int peer(int);
extern "C" int *peer_counter();
int main() {
  if (tracked(1) != 9 || peer(4) != 18) return 1;
  int &counter = shared<int>();
  int *other = peer_counter();
  if (&counter != other) return 2;
  ++counter;
  *other += 2;
  return counter == 3 ? 0 : 3;
}
#endif
