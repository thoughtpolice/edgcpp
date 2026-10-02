#include <array>
#include <bit>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>

static_assert(CHAR_BIT == 8);
static_assert(sizeof(std::uint32_t) == 4);
static_assert(std::numeric_limits<std::uint32_t>::digits == 32);
static_assert(std::popcount(std::uint32_t{0x12345678}) == 13);
static_assert(std::rotl(std::uint32_t{0x12345678}, 8) == 0x34567812);

int main() {
  volatile std::uint32_t source = 0x12345678;
  const std::uint32_t value = source;
  const std::uint32_t rotated = std::rotl(value, 8);
  if (std::popcount(value) != 13 || rotated != 0x34567812 ||
      std::rotr(rotated, 8) != value || std::rotl(value, -8) != 0x78123456 ||
      std::rotl(value, 32) != value || std::rotr(value, 64) != value)
    return 1;

  volatile std::uint32_t zero_source = 0;
  volatile std::uint32_t sparse_source = 0x00f01000;
  const std::uint32_t zero = zero_source;
  const std::uint32_t sparse = sparse_source;
  if (std::countr_zero(sparse) != 12 || std::countl_zero(sparse) != 8 ||
      std::countr_zero(zero) != 32 || std::countl_zero(zero) != 32 ||
      std::popcount(zero) != 0 ||
      std::countl_one(std::uint32_t{0xffff0000}) != 16 ||
      std::countr_one(std::uint32_t{0x0000ffff}) != 16)
    return 2;
  if (std::bit_width(value) != 29 || std::bit_floor(value) != 0x10000000 ||
      std::bit_ceil(value) != 0x20000000 || std::bit_width(zero) != 0 ||
      std::bit_floor(zero) != 0 || std::bit_ceil(zero) != 1 ||
      !std::has_single_bit(std::rotl(std::uint32_t{1}, 31)) ||
      std::rotl(std::uint32_t{1}, 31) != 0x80000000 ||
      std::has_single_bit(value) || std::has_single_bit(zero))
    return 3;

  auto bytes = std::bit_cast<std::array<std::byte, 4>>(value);
  if (std::bit_cast<std::uint32_t>(bytes) != value)
    return 4;
  std::array<std::byte, 4> canonical;
  if constexpr (std::endian::native == std::endian::little) {
    if (bytes[0] != std::byte{0x78} || bytes[1] != std::byte{0x56} ||
        bytes[2] != std::byte{0x34} || bytes[3] != std::byte{0x12})
      return 5;
    canonical = {bytes[3], bytes[2], bytes[1], bytes[0]};
    bytes[0] = std::byte{0xf0};
  } else if constexpr (std::endian::native == std::endian::big) {
    if (bytes[0] != std::byte{0x12} || bytes[1] != std::byte{0x34} ||
        bytes[2] != std::byte{0x56} || bytes[3] != std::byte{0x78})
      return 6;
    canonical = bytes;
    bytes[3] = std::byte{0xf0};
  } else {
    return 7;
  }
  const auto changed = std::bit_cast<std::uint32_t>(bytes);
  if (changed != 0x123456f0)
    return 8;
  std::printf("bits=%d rotate=%08x zeros=%d/%d bytes=%02x%02x%02x%02x changed=%08x\n",
              std::popcount(value), static_cast<unsigned>(rotated),
              std::countr_zero(sparse), std::countr_zero(zero),
              std::to_integer<unsigned>(canonical[0]),
              std::to_integer<unsigned>(canonical[1]),
              std::to_integer<unsigned>(canonical[2]),
              std::to_integer<unsigned>(canonical[3]), static_cast<unsigned>(changed));
}
