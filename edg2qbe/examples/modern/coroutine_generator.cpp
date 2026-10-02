#include <coroutine>
#include <cstdio>
#include <exception>
#include <utility>

class Generator {
public:
    struct promise_type {
        int current = 0;
        std::exception_ptr failure;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int value) noexcept {
            current = value;
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() noexcept { failure = std::current_exception(); }
    };

    using Handle = std::coroutine_handle<promise_type>;
    explicit Generator(Handle handle) noexcept : handle_(handle) {}
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept
        : handle_(std::exchange(other.handle_, {})) {}
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }
    ~Generator() { if (handle_) handle_.destroy(); }

    bool next() {
        if (!handle_ || handle_.done()) return false;
        handle_.resume();
        if (handle_.promise().failure)
            std::rethrow_exception(handle_.promise().failure);
        return !handle_.done();
    }
    int value() const { return handle_.promise().current; }

private:
    Handle handle_;
};

struct Stats {
    int started = 0;
    int alive = 0;
    int destroyed = 0;
    int steps = 0;
};

struct Guard {
    Stats& stats;
    explicit Guard(Stats& stats) : stats(stats) {
        ++stats.started;
        ++stats.alive;
    }
    ~Guard() { --stats.alive; ++stats.destroyed; }
};

struct Failure { int index; };

Generator generate(Stats& stats, int count, int fail_at = -1) {
    Guard guard{stats};
    for (int index = 0; index != count; ++index) {
        if (index == fail_at) throw Failure{index};
        ++stats.steps;
        co_yield (index + 1) * 3;
    }
}

int main() {
    Stats complete;
    int values[3]{};
    int sum = 0;
    {
        auto generator = generate(complete, 3);
        if (complete.started != 0 || complete.steps != 0) return 1;
        for (int index = 0; index != 3; ++index) {
            if (!generator.next()) return 2;
            values[index] = generator.value();
            if (values[index] != (index + 1) * 3 || complete.alive != 1 ||
                complete.steps != index + 1 || complete.destroyed != 0) return 3;
            sum += values[index];
        }
        if (generator.next() || generator.next() || complete.alive != 0 ||
            complete.started != 1 || complete.destroyed != 1) return 4;
    }
    if (sum != 18 || complete.destroyed != 1) return 5;

    Stats early;
    {
        auto generator = generate(early, 5);
        if (!generator.next() || generator.value() != 3 || early.alive != 1)
            return 6;
        auto moved = std::move(generator);
        if (generator.next() || !moved.next() || moved.value() != 6 ||
            early.steps != 2 || early.destroyed != 0) return 7;
        // The moved owner is destroyed while suspended at its second yield.
    }
    if (early.started != 1 || early.alive != 0 || early.destroyed != 1 ||
        early.steps != 2) return 8;

    Stats replacement;
    Stats source;
    {
        auto destination = generate(replacement, 4);
        auto incoming = generate(source, 4);
        if (!destination.next() || !incoming.next()) return 9;
        destination = std::move(incoming);
        if (replacement.alive != 0 || replacement.destroyed != 1 ||
            incoming.next() || !destination.next() || destination.value() != 6)
            return 10;
    }
    if (source.alive != 0 || source.destroyed != 1 || source.steps != 2)
        return 11;

    Stats exceptional;
    int caught_index = -1;
    {
        auto generator = generate(exceptional, 4, 2);
        if (!generator.next() || generator.value() != 3 || !generator.next() ||
            generator.value() != 6) return 12;
        try {
            generator.next();
            return 13;
        } catch (const Failure& failure) {
            caught_index = failure.index;
        } catch (...) {
            return 14;
        }
        if (caught_index != 2 || exceptional.alive != 0 ||
            exceptional.destroyed != 1 || exceptional.steps != 2 ||
            generator.next()) return 15;
    }
    if (exceptional.destroyed != 1) return 16;
    std::printf("coroutine values=%d,%d,%d sum=%d early=%d/%d replacement=%d/%d exception=%d/%d\n",
                values[0], values[1], values[2], sum, early.steps, early.destroyed,
                replacement.destroyed, source.destroyed, caught_index,
                exceptional.destroyed);
}
