#include <charconv>
#include <cstdio>
#include <expected>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

enum class Error : unsigned char { malformed = 3, negative = 7, overflow = 11 };
using Result = std::expected<int, Error>;

Result parse(std::string_view text) {
  int value = 0;
  const auto converted = std::from_chars(text.data(), text.data() + text.size(), value);
  if (converted.ec == std::errc::result_out_of_range)
    return std::unexpected(Error::overflow);
  if (converted.ec != std::errc{} || converted.ptr != text.data() + text.size())
    return std::unexpected(Error::malformed);
  if (value < 0)
    return std::unexpected(Error::negative);
  return value;
}

int main() {
  int continuations = 0;
  int transformations = 0;
  int recoveries = 0;
  const auto pipeline = [&](std::string_view text) {
    return parse(text)
        .and_then([&](int value) -> Result {
          ++continuations;
          if (value > (std::numeric_limits<int>::max() - 1) / 2)
            return std::unexpected(Error::overflow);
          return value * 2;
        })
        .transform([&](int value) {
          ++transformations;
          return value + 1;
        })
        .or_else([&](Error error) -> Result {
          ++recoveries;
          if (error == Error::negative)
            return 5;
          return std::unexpected(error);
        });
  };

  auto good = pipeline("21");
  auto recovered = pipeline("-2");
  auto malformed = pipeline("not-a-number");
  auto trailing = pipeline("12tail");
  auto overflow = pipeline(std::to_string(std::numeric_limits<int>::max()));
  auto zero = pipeline("0");
  if (!good || *good != 43 || !recovered || *recovered != 5 || !zero || *zero != 1)
    return 1;
  if (malformed || malformed.error() != Error::malformed || trailing ||
      trailing.error() != Error::malformed || overflow || overflow.error() != Error::overflow)
    return 2;
  if (continuations != 3 || transformations != 2 || recoveries != 4)
    return 3;

  int bad_value = 0;
  try {
    (void)malformed.value();
    return 4;
  } catch (const std::bad_expected_access<Error>& error) {
    if (error.error() != Error::malformed)
      return 5;
    bad_value = 1;
  }
  auto translated = malformed.transform_error([](Error error) {
    return static_cast<int>(std::to_underlying(error)) + 100;
  });
  auto translated_good = good.transform_error([](Error error) {
    return static_cast<int>(std::to_underlying(error)) + 100;
  });
  if (translated || translated.error() != 103 || !translated_good || *translated_good != 43)
    return 6;

  Result editable(4);
  auto edited = editable.transform([](int& value) {
    value += 2;
    return value * 3;
  });
  if (*editable != 6 || !edited || *edited != 18)
    return 7;
  auto void_chain = parse("0")
                        .transform([](int) {})
                        .and_then([]() -> Result { return 77; });
  if (!void_chain || *void_chain != 77)
    return 8;

  std::expected<std::unique_ptr<int>, Error> owned(std::in_place, std::make_unique<int>(9));
  auto moved = std::move(owned)
                   .and_then([](std::unique_ptr<int> pointer)
                                 -> std::expected<std::unique_ptr<int>, Error> {
                     *pointer += 4;
                     return std::move(pointer);
                   })
                   .transform([](std::unique_ptr<int> pointer) { return *pointer * 3; });
  if (!owned || *owned || !moved || *moved != 39)
    return 9;
  if (std::to_underlying(Error::malformed) != 3 || std::to_underlying(Error::overflow) != 11)
    return 10;
  std::printf("expected=%d,%d,%d errors=%u,%u moved=%d monadic=%d/%d/%d bad_value=%d\n",
              *good, *recovered, *zero,
              static_cast<unsigned>(std::to_underlying(malformed.error())),
              static_cast<unsigned>(std::to_underlying(overflow.error())), *moved,
              continuations, transformations, recoveries, bad_value);
}
