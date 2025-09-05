#pragma once
#include <array>
#include <cstdint>

using uint128_t = unsigned __int128;

namespace detail {

/// Build 16 partial carryless products of x with nibble n in [0,15].
/// Each entry is XOR of up to 4 shifted copies of x (bits of the nibble).
constexpr std::array<uint128_t, 16> ClMulNibbleTable(std::uint64_t x) noexcept {
  const auto a0 = uint128_t{x};
  const auto a1 = a0 << 1;
  const auto a2 = a0 << 2;
  const auto a3 = a0 << 3;

  std::array<uint128_t, 16> t;
  t[0] = uint128_t{0};
  // n = b0 + 2*b1 + 4*b2 + 8*b3 => XOR the selected shifted copies.
  for (int n = 1; n != 16; ++n) {
    auto r = uint128_t{0};
    if (n & 0x1) r ^= a0;
    if (n & 0x2) r ^= a1;
    if (n & 0x4) r ^= a2;
    if (n & 0x8) r ^= a3;
    t[n] = r;
  }
  return t;
} // ClMulNibbleTable

} // detail

/// Carryless-multiply two 64-bit ints using a 4-bit table (scan y once).
/// Complexity: up to 16 XORs and shifts; no nested nibble loops.
constexpr uint128_t ClMul(std::uint64_t x, std::uint64_t y) noexcept {
  const auto tbl = detail::ClMulNibbleTable(x);
  auto r = uint128_t{0};
  for (int j = 0; y != 0; ++j, y>>=4) {
    const auto ny = static_cast<std::uint8_t>(y & 0x0f);
    r ^= (tbl[ny] << (4 * j));
  }
  return r;
} // ClMul
