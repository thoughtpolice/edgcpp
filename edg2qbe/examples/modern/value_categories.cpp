#include <cstdio>
#include <memory>
#include <type_traits>
#include <utility>

struct Qualified {
    int category() & { return 1; }
    int category() const & { return 2; }
    int category() && { return 3; }
    int category() const && { return 4; }
};

template<class T>
int forwarded_category(T&& value) {
    return std::forward<T>(value).category();
}

struct Immobile {
    int value;
    int* alive;
    int* destroyed;
    Immobile(int value, int& alive, int& destroyed)
        : value(value), alive(&alive), destroyed(&destroyed) { ++alive; }
    Immobile(const Immobile&) = delete;
    Immobile(Immobile&&) = delete;
    ~Immobile() { --*alive; ++*destroyed; }
};

Immobile make_immobile(int& alive, int& destroyed) {
    // This prvalue constructs the caller's result object directly, even
    // though neither a copy nor a move constructor exists.
    return Immobile{42, alive, destroyed};
}

struct Lifetime {
    int* alive;
    int* destroyed;
    int value;
    Lifetime(int& alive, int& destroyed, int value)
        : alive(&alive), destroyed(&destroyed), value(value) { ++alive; }
    ~Lifetime() { --*alive; ++*destroyed; }
};

int main() {
    Qualified mutable_value;
    const Qualified const_value;
    const int lvalue = forwarded_category(mutable_value);
    const int const_lvalue = forwarded_category(const_value);
    const int rvalue = forwarded_category(std::move(mutable_value));
    const int const_rvalue = forwarded_category(std::move(const_value));
    if (lvalue != 1 || const_lvalue != 2 || rvalue != 3 || const_rvalue != 4)
        return 1;

    auto accumulate = [value = std::make_unique<int>(5)](int delta) mutable {
        *value += delta;
        return *value;
    };
    static_assert(!std::is_copy_constructible_v<decltype(accumulate)>);
    static_assert(std::is_move_constructible_v<decltype(accumulate)>);
    auto owner = std::move(accumulate);
    const int first = owner(3);
    const int second = owner(2);
    if (first != 8 || second != 10) return 2;

    int alive = 0;
    int destroyed = 0;
    int elided_value = 0;
    {
        Immobile result = make_immobile(alive, destroyed);
        if (alive != 1 || destroyed != 0) return 3;
        elided_value = result.value;
    }
    if (alive != 0 || destroyed != 1 || elided_value != 42) return 4;

    destroyed = 0;
    int lifetime_sum = 0;
    {
        const Lifetime& complete = Lifetime{alive, destroyed, 11};
        if (alive != 1 || destroyed != 0) return 5;
        // Binding to a subobject also extends the complete object's lifetime.
        const int& member = Lifetime{alive, destroyed, 13}.value;
        if (alive != 2 || destroyed != 0) return 6;
        lifetime_sum = complete.value + member;
        if (lifetime_sum != 24) return 7;
    }
    if (alive != 0 || destroyed != 2) return 8;
    std::printf("categories=%d,%d,%d,%d capture=%d,%d elision=%d lifetime=%d/%d\n",
                lvalue, const_lvalue, rvalue, const_rvalue, first, second,
                elided_value, lifetime_sum, destroyed);
}
