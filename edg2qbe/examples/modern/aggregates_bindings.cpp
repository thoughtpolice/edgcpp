#include <cstdio>
#include <type_traits>

struct Point {
    int x = 4;
    int y = 6;
};

struct Packet {
    Point position;
    int weights[3];
    int extra = 9;
};

struct EmptyPolicy {
    constexpr int apply(int value) const { return value * 2; }
};

struct Wrapped {
    [[no_unique_address]] EmptyPolicy policy;
    int value;
    int evaluate() const { return policy.apply(value); }
};

struct TailPolicy {
    int value;
    char tag;
    TailPolicy(int initial = 0, char marker = 0) : value(initial), tag(marker) {}
};

struct TailWrapped {
    [[no_unique_address]] TailPolicy policy;
    char adjacent;
};

int main() {
    static_assert(std::is_aggregate_v<Point>);
    static_assert(std::is_aggregate_v<Packet>);
    static_assert(std::is_aggregate_v<Wrapped>);
    Packet packet{.position = {.y = 8}, .weights = {2, 3}};
    if (packet.position.x != 4 || packet.position.y != 8 ||
        packet.weights[2] != 0 || packet.extra != 9) return 1;

    auto [copy_x, copy_y] = packet.position;
    copy_x = 99;
    copy_y = 100;
    if (packet.position.x != 4 || packet.position.y != 8) return 2;
    auto& [ref_x, ref_y] = packet.position;
    static_assert(std::is_same_v<decltype(ref_x), int>);
    static_assert(std::is_same_v<decltype((ref_x)), int&>);
    ref_x += 5;
    ref_y += 2;
    if (&ref_x != &packet.position.x || &ref_y != &packet.position.y ||
        packet.position.x != 9 || packet.position.y != 10) return 3;

    const auto& [view_x, view_y] = packet.position;
    static_assert(std::is_same_v<decltype((view_x)), const int&>);
    ref_x += 1;
    if (view_x != 10 || view_y != 10) return 4;
    auto& [first, second, third] = packet.weights;
    first += second;
    third = 7;
    if (packet.weights[0] != 5 || packet.weights[1] != 3 ||
        packet.weights[2] != 7) return 5;

    Wrapped wrapped{.value = 21};
    Wrapped other{.value = 5};
    const int policy_result = wrapped.evaluate();
    if (policy_result != 42 || other.evaluate() != 10) return 6;
    wrapped.value = 23;
    if (wrapped.evaluate() != 46 || other.value != 5) return 7;
    wrapped.policy = other.policy;
    other.policy = EmptyPolicy{};
    Wrapped empty_copy = wrapped;
    Wrapped empty_assigned{.value = 1};
    empty_assigned = other;
    if (wrapped.value != 23 || other.value != 5 ||
        empty_copy.evaluate() != 46 || empty_assigned.evaluate() != 10) return 8;

    TailWrapped tail_source{{31, 's'}, 'S'};
    TailWrapped tail_target{{17, 't'}, 'T'};
    tail_target.policy = tail_source.policy;
    if (tail_target.policy.value != 31 || tail_target.policy.tag != 's' ||
        tail_target.adjacent != 'T' || tail_source.adjacent != 'S') return 9;
    tail_target.policy = TailPolicy{47, 'r'};
    if (tail_target.policy.value != 47 || tail_target.policy.tag != 'r' ||
        tail_target.adjacent != 'T') return 10;
    TailPolicy complete_copy = tail_target.policy;
    if (complete_copy.value != 47 || complete_copy.tag != 'r') return 11;
    complete_copy.value = 59;
    complete_copy.tag = 'c';
    if (tail_target.policy.value != 47 || tail_target.policy.tag != 'r' ||
        tail_target.adjacent != 'T') return 12;
    tail_source.policy = complete_copy;
    if (tail_source.policy.value != 59 || tail_source.policy.tag != 'c' ||
        tail_source.adjacent != 'S') return 13;
    TailWrapped tail_copy = tail_source;
    tail_target = tail_copy;
    if (tail_target.policy.value != 59 || tail_target.policy.tag != 'c' ||
        tail_target.adjacent != 'S' || tail_copy.policy.value != 59 ||
        tail_copy.policy.tag != 'c' || tail_copy.adjacent != 'S') return 14;
    // No size, padding, member-address, or overlap assumption is made about
    // [[no_unique_address]]: it must preserve the normal object behavior.
    std::printf("aggregates copies=%d,%d refs=%d,%d weights=%d,%d,%d extra=%d policy=%d\n",
                copy_x, copy_y, view_x, view_y, first, second, third,
                packet.extra, policy_result);
}
