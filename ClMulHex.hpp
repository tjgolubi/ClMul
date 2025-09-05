#pragma once
#include <array>
#include <utility>
#include <cstdint>

using uint128_t = unsigned __int128;

namespace detail {

/// Carryless-multiply two 64-bit integers using 4-bit nibbles and a 16x16 table.
/// Produces a 128-bit XOR-accumulated product over GF(2).
constexpr std::uint8_t ClMulNibble(std::uint8_t a, std::uint8_t b) noexcept {
  auto r = std::uint8_t{0};
  if (a < b)
    std::swap(a, b);
  for ( ; b != 0; b>>=1, a<<=1) {
    if (b & 1)
      r ^= a;
  }
  return r;
} // ClMulNibble

using Row   = std::array<std::uint8_t, 16>;
using Table = std::array<Row, 16>;

constexpr Table MakeCl4Table() noexcept {
  auto t = Table{};
  t[0][0] = t[0][1] = t[1][0] = std::uint8_t{0};
  t[1][1] = std::uint8_t{1};
  for (std::uint8_t a = 2; a != 16; ++a) {
    t[0][a] = t[a][0] = 0;
    t[1][a] = t[a][1] = a;
    t[a][a] = ClMulNibble(a, a);
  }
  for (std::uint8_t a = 3; a != 16; ++a) {
    for (std::uint8_t b = 2; b != a; ++b)
      t[a][b] = t[b][a] = ClMulNibble(a, b);
  }
  return t;
} // MakeCl4Table

constinit const auto KCl4Table = MakeCl4Table();

} // detail

constexpr uint128_t ClMulHex(uint64_t x, uint64_t y) noexcept {
  auto r = uint128_t{0};
  for (int i = 0; x != 0; ++i, x>>=4) {
    const auto xi = static_cast<std::uint8_t>(x & 0x0f);
    if (xi == 0)
      continue;
    const auto& row = detail::KCl4Table[xi];
    auto z = y;
    for (int j = 0; z != 0; ++j, z>>=4) {
      const std::uint8_t yj = static_cast<std::uint8_t>(z & 0x0f);
      if (yj == 0)
        continue;
      const std::uint8_t p = row[yj];
      r ^= static_cast<uint128_t>(p) << (4 * (i + j));
    }
  }
  return r;
} // ClMulHex
