#include <concepts>
#include <cstdio>
#include <type_traits>

// Reusing the named atomic constraint is what makes the third overload
// more constrained than the second one, rather than merely equivalent.
template<class T> concept Integer = std::integral<T>;
template<class T> concept SignedInteger = Integer<T> && std::is_signed_v<T>;

template<class T> constexpr int select(T) { return 0; }
template<Integer T> constexpr int select(T) { return 10; }
template<SignedInteger T> constexpr int select(T) { return 20; }

template<class T>
concept HasValue = requires(const T& value) {
    { value.get() } -> std::convertible_to<int>;
};
template<class T>
concept IncrementableValue = HasValue<T> && requires(T& value) {
    { value.increment() } -> std::same_as<void>;
};

struct ReadOnly {
    int value;
    int get() const { return value; }
};
struct Mutable {
    int value;
    int get() const { return value; }
    void increment() { ++value; }
};

template<HasValue T> int inspect(T& value) { return value.get(); }
template<IncrementableValue T> int inspect(T& value) {
    value.increment();
    return value.get() * 2;
}

int main() {
    static_assert(select(1.0) == 0);
    static_assert(select(1u) == 10);
    static_assert(select(1) == 20);
    ReadOnly read_only{7};
    Mutable mutable_value{7};
    const int plain = inspect(read_only);
    const int modified = inspect(mutable_value);
    if (plain != 7 || modified != 16 || mutable_value.value != 8) return 1;

    int digits = 0;
    int total = 0;
    int steps = 0;
    auto sequence = [&]<Integer... Ts>(Ts... values)
        requires (sizeof...(Ts) > 0) {
        auto append = [&](int value) {
            digits = digits * 10 + value;
            total += value;
            ++steps;
        };
        // The comma fold guarantees left-to-right sequencing of append calls.
        (append(values), ...);
        return total;
    };
    static_assert(std::invocable<decltype(sequence), int, unsigned>);
    static_assert(!std::invocable<decltype(sequence)>);
    static_assert(!std::invocable<decltype(sequence), double>);
    const int sum = sequence(1, 2u, 3, 4u);
    if (digits != 1234 || sum != 10 || steps != 4) return 2;
    std::printf("concepts picks=%d,%d,%d members=%d,%d fold=%d sum=%d\n",
                select(1.0), select(1u), select(1), plain, modified, digits, sum);
}
