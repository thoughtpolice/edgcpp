#include <cstdio>

struct Counts {
    int live = 0;
    int constructed = 0;
    int destroyed = 0;
    int order = 0;
    int arguments_live = 0;
    int arguments_destroyed = 0;
};

struct Argument {
    Counts& counts;
    bool fail;
    Argument(Counts& counts, bool fail) : counts(counts), fail(fail) {
        ++counts.arguments_live;
    }
    ~Argument() noexcept(false) {
        --counts.arguments_live;
        ++counts.arguments_destroyed;
        if (fail) throw 17;
    }
};

struct Object {
    Counts& counts;
    int value;
    Object(Counts& counts, int value) : counts(counts), value(value) {
        ++counts.live;
        ++counts.constructed;
    }
    Object(Counts& counts, int value, const Argument&, bool fail)
        : counts(counts), value(value) {
        if (fail) throw 19;
        ++counts.live;
        ++counts.constructed;
    }
    ~Object() {
        --counts.live;
        ++counts.destroyed;
        counts.order = counts.order * 10 + value;
    }
};

int binding(Counts& counts, int mode) {
    Object enclosing(counts, 1);
    const int& member = Object(counts, 2, Argument(counts, mode == 2),
                               mode == 3).value;
    if (counts.live != 2 || counts.destroyed != 0 ||
        counts.arguments_live != 0 || counts.arguments_destroyed != 1 ||
        member != 2) return -1;
    if (mode == 1) throw 31;
    return member;
}

int main() {
    for (int mode = 0; mode != 4; ++mode) {
        Counts counts;
        int outcome = 0;
        try {
            outcome = binding(counts, mode);
        } catch (int code) {
            outcome = code;
        } catch (...) {
            return 10 + mode;
        }
        const int expected = mode == 0 ? 2 : mode == 1 ? 31 :
                             mode == 2 ? 17 : 19;
        const int objects = mode == 3 ? 1 : 2;
        if (outcome != expected || counts.live != 0 ||
            counts.constructed != objects || counts.destroyed != objects ||
            counts.order != (mode == 3 ? 1 : 21) ||
            counts.arguments_live != 0 || counts.arguments_destroyed != 1)
            return 20 + mode;
    }
    std::puts("subobject-lifetime return=2 throw=31 argument=17 constructor=19 order=21");
}
