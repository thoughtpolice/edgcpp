#include <array>
#include <cstdio>
#include <type_traits>

consteval int square(int value) { return value * value; }

constexpr int transform(int value) {
    if consteval {
        return square(value) + 1;
    } else {
        return value * 3 + 1;
    }
}

constexpr int factorial(int value) {
    int result = 1;
    for (int n = 2; n <= value; ++n) result *= n;
    return result;
}

struct Settings {
    int bias;
    unsigned stride;
    constexpr bool operator==(const Settings&) const = default;
};

template<Settings Config>
struct Table {
    std::array<int, 4> values{};
    constexpr explicit Table(int base) {
        for (unsigned index = 0; index != values.size(); ++index)
            values[index] = base + Config.bias + int(index * Config.stride);
    }
    constexpr int sum() const {
        int result = 0;
        for (int value : values) result += value;
        return result;
    }
};

// Structural NTTP equality must identify the same specialization.
using Sequence = Table<Settings{3, 2}>;
using SameSequence = Table<Settings{3, 2}>;
static_assert(std::is_same_v<Sequence, SameSequence>);
constexpr Sequence compile_table{4};
static_assert(compile_table.values[0] == 7);
static_assert(compile_table.values[3] == 13);
static_assert(compile_table.sum() == 40);
constexpr int compile_transform = transform(7);
static_assert(compile_transform == 50);
static_assert(factorial(6) == 720);
constinit int mutable_counter = square(4);

int main() {
    // Volatile reads prevent these calls from being constant expressions.
    volatile int input = 7;
    const int runtime_transform = transform(input);
    const SameSequence runtime_table{input + 1};
    volatile int factorial_input = 5;
    const int runtime_factorial = factorial(factorial_input);
    if (runtime_transform != 22 || runtime_table.values[0] != 11 ||
        runtime_table.values[3] != 17 || runtime_table.sum() != 56 ||
        runtime_factorial != 120 || mutable_counter != 16) return 1;
    mutable_counter += runtime_transform;
    if (mutable_counter != 38) return 2;
    std::printf("constants branch=%d,%d table=%d,%d factorial=%d counter=%d\n",
                compile_transform, runtime_transform, compile_table.sum(),
                runtime_table.sum(), runtime_factorial, mutable_counter);
}
