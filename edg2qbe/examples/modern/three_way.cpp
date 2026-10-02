#include <compare>
#include <cstdio>
#include <limits>
#include <type_traits>

struct Key {
    int major;
    int minor;
    auto operator<=>(const Key&) const = default;
};

struct Reading {
    double value;
    int channel;
    auto operator<=>(const Reading&) const = default;
};

// A non-member candidate with reversed operands tests rewritten candidates,
// not just relational operators synthesized from a same-type member.
struct Boundary {
    int value;
    friend std::strong_ordering operator<=>(const Boundary& a, int b) {
        return a.value <=> b;
    }
    friend bool operator==(const Boundary& a, int b) { return a.value == b; }
};

int main() {
    static_assert(std::is_same_v<decltype(Key{} <=> Key{}), std::strong_ordering>);
    static_assert(std::is_same_v<decltype(Reading{} <=> Reading{}), std::partial_ordering>);
    constexpr Key first{2, 3};
    constexpr Key later{2, 4};
    constexpr Key next_major{3, 0};
    static_assert(first < later && later < next_major);
    static_assert(first == Key{2, 3} && first != later);
    if (!(first < later) || !(later > first) || !(first <= first) ||
        !(later >= first) || !(first != later) || !(first == Key{2, 3})) return 1;

    const Boundary boundary{7};
    if (!(boundary < 8) || !(8 > boundary) || !(6 < boundary) ||
        !(boundary > 6) || !(7 == boundary) || !(boundary == 7) ||
        (7 != boundary) || !(8 != boundary)) return 2;

    const Reading low{2.5, 1};
    const Reading high{2.5, 2};
    if (!((low <=> high) == std::partial_ordering::less) || !(low < high) ||
        !(high > low) || !(low == Reading{2.5, 1})) return 3;
    const Reading unknown{std::numeric_limits<double>::quiet_NaN(), 0};
    if (!((unknown <=> low) == std::partial_ordering::unordered) ||
        !((low <=> unknown) == std::partial_ordering::unordered)) return 4;
    if (unknown == low || !(unknown != low) || unknown < low ||
        unknown > low || unknown <= low || unknown >= low ||
        low < unknown || low > unknown || low <= unknown || low >= unknown)
        return 5;
    // Even self-comparison is unordered for the NaN-containing aggregate.
    if (unknown == unknown || !(unknown != unknown)) return 6;
    std::printf("comparison lexicographic=%d rewritten=%d partial=%d unordered=%d\n",
                first < later, 8 > boundary, low < high,
                (unknown <=> low) == std::partial_ordering::unordered);
}
