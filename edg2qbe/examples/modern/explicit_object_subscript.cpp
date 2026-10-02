#include <array>
#include <cstddef>
#include <cstdio>
#include <type_traits>
#include <utility>

struct Grid {
    std::array<int, 6> cells;
    constexpr int& operator[](std::size_t row, std::size_t column) noexcept {
        return cells[row * 3 + column];
    }
    constexpr const int& operator[](std::size_t row, std::size_t column) const noexcept {
        return cells[row * 3 + column];
    }
};

struct Accumulator {
    int total = 0;

    template<class Self>
    decltype(auto) value(this Self&& self) noexcept {
        return (std::forward<Self>(self).total);
    }

    template<class Self>
    auto&& add(this Self&& self, int amount) noexcept {
        self.total += amount;
        return std::forward<Self>(self);
    }
};

constexpr int compile_grid() {
    Grid grid{{1, 2, 3, 4, 5, 6}};
    grid[1, 2] += 4;
    const Grid& view = grid;
    return view[1, 2] + view[0, 0];
}
static_assert(compile_grid() == 11);
static_assert(std::is_same_v<decltype(std::declval<Accumulator&>().value()), int&>);
static_assert(std::is_same_v<decltype(std::declval<const Accumulator&>().value()), const int&>);
static_assert(std::is_same_v<decltype(std::declval<Accumulator&&>().value()), int&&>);
static_assert(std::is_same_v<decltype(std::declval<const Accumulator&&>().value()), const int&&>);
static_assert(std::is_same_v<decltype(std::declval<Accumulator&>().add(1)), Accumulator&>);
static_assert(std::is_same_v<decltype(std::declval<Accumulator&&>().add(1)), Accumulator&&>);

int main() {
    Accumulator accumulator{10};
    auto& alias = accumulator.add(5);
    if (&alias != &accumulator) return 1;
    int& storage = accumulator.value();
    storage += 2;
    const Accumulator& const_view = accumulator;
    if (const_view.value() != 17 || &storage != &accumulator.total) return 2;
    // The returned rvalue reference is consumed before the temporary dies.
    auto owned = Accumulator{3}.add(4).add(6);
    if (owned.value() != 13) return 3;
    auto&& rvalue_alias = std::move(accumulator).add(0);
    if (&rvalue_alias != &accumulator || rvalue_alias.value() != 17) return 4;

    Grid grid{{1, 2, 3, 4, 5, 6}};
    grid[1, 2] = 9;
    grid[1, 2] += 4;
    grid[0, 1] = const_view.value();
    const Grid& read_only = grid;
    if (read_only[1, 2] != 13 || read_only[0, 1] != 17) return 5;
    int sum = 0;
    for (std::size_t row = 0; row != 2; ++row)
        for (std::size_t column = 0; column != 3; ++column)
            sum += read_only[row, column];
    if (sum != 43) return 6;
    std::printf("explicit-object grid=%d accumulator=%d owned=%d\n",
                sum, const_view.value(), owned.value());
}
