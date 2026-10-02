#include <cstdio>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

struct Tracked {
  int* live;
  int value;

  Tracked(int value, int& live) : live(&live), value(value) { ++live; }
  Tracked(const Tracked&) = delete;
  Tracked& operator=(const Tracked&) = delete;
  Tracked(Tracked&& other) noexcept
      : live(other.live), value(std::exchange(other.value, -1)) {
    ++*live;
  }
  Tracked& operator=(Tracked&& other) noexcept {
    value = std::exchange(other.value, -1);
    return *this;
  }
  ~Tracked() { --*live; }
};

int main() {
  int live = 0;
  int bad_accesses = 0;
  int optional_value = 0;
  int moved_value = 0;
  int variant_value = 0;
  {
    std::optional<Tracked> original;
    if (original || live != 0)
      return 1;
    try {
      (void)original.value();
      return 2;
    } catch (const std::bad_optional_access&) {
      ++bad_accesses;
    }
    original.emplace(7, live);
    if (!original || original->value != 7 || live != 1)
      return 3;
    std::optional<Tracked> moved(std::move(original));
    if (!original || !moved || original->value != -1 || moved->value != 7 ||
        live != 2)
      return 4;
    original.reset();
    if (original || live != 1)
      return 5;
    moved.emplace(17, live);
    if (moved->value != 17 || live != 1)
      return 6;
    optional_value = moved->value;
    moved.reset();
    if (live != 0)
      return 7;

    using Value = std::variant<std::monostate, Tracked, std::unique_ptr<int>>;
    Value value;
    const auto read = [](const auto& item) -> int {
      using T = std::remove_cvref_t<decltype(item)>;
      if constexpr (std::is_same_v<T, std::monostate>)
        return 0;
      else if constexpr (std::is_same_v<T, Tracked>)
        return item.value;
      else
        return item ? *item : -1;
    };
    if (value.index() != 0 || std::visit(read, value) != 0)
      return 8;
    value.emplace<1>(9, live);
    if (!std::holds_alternative<Tracked>(value) || live != 1 ||
        std::get_if<Tracked>(&value)->value != 9 ||
        std::get_if<std::unique_ptr<int>>(&value) != nullptr)
      return 9;
    try {
      (void)std::get<2>(value);
      return 10;
    } catch (const std::bad_variant_access&) {
      ++bad_accesses;
    }
    Value moved_variant(std::move(value));
    if (live != 2 || value.index() != 1 || std::visit(read, value) != -1 ||
        std::visit(read, moved_variant) != 9)
      return 11;
    moved_value = std::visit(read, moved_variant);
    value.emplace<0>();
    if (live != 1)
      return 12;
    moved_variant.emplace<2>(std::make_unique<int>(42));
    if (live != 0 || moved_variant.valueless_by_exception())
      return 13;
    value = std::move(moved_variant);
    if (value.index() != 2 || moved_variant.index() != 2 ||
        std::get<2>(moved_variant) || std::visit(read, value) != 42)
      return 14;
    variant_value = std::visit(read, value);
    const int total = std::visit(
        [&](const auto& left, const auto& right) { return read(left) + read(right); },
        value, moved_variant);
    if (total != 41)
      return 15;
    value.emplace<0>();
    moved_variant.emplace<0>();
  }
  if (live != 0 || bad_accesses != 2)
    return 16;
  std::printf("optional=%d variant=%d moved=%d bad=%d live=%d\n", optional_value,
              variant_value, moved_value, bad_accesses, live);
}
