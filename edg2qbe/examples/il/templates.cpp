template<class T>
concept Addable = requires(T a, T b) { a + b; };

template<Addable T, int N>
struct Buffer {
  T data[N];

  constexpr T sum() const {
    T result{};
    for (const auto& value : data) {
      result = result + value;
    }
    return result;
  }
};

constexpr Buffer<int, 3> values{{1, 2, 3}};
static_assert(values.sum() == 6);

int main() {
  int offset = 4;
  auto adjust = [offset]<Addable T>(T value) { return value + offset; };
  return adjust(values.sum()) == 10 ? 0 : 1;
}
