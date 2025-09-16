#pragma once
#include <array>
#include <cstdint>

namespace tjg {

using uint128_t = unsigned __int128;

namespace detail {

/// Build 16 partial carryless products of x with nibble n in [0,15].
/// Each entry is XOR of up to 4 shifted copies of x (bits of the nibble).
constexpr std::array<uint128_t, 16> ClMulNibbleTable(std::uint64_t x) noexcept {
  const auto a0 = uint128_t{x};
  const auto a1 = a0 << 1;
  const auto a2 = a0 << 2;
  const auto a3 = a0 << 3;
  return std::array<uint128_t, 16>{
                    0,
                   a0,
              a1     ,
              a1 ^ a0,
         a2          ,
         a2      ^ a0,
         a2 ^ a1     ,
         a2 ^ a1 ^ a0,
    a3               ,
    a3           ^ a0,
    a3      ^ a1     ,
    a3      ^ a1 ^ a0,
    a3 ^ a2          ,
    a3 ^ a2      ^ a0,
    a3 ^ a2 ^ a1     ,
    a3 ^ a2 ^ a1 ^ a0
  };
} // ClMulNibbleTable

} // detail

/// Carryless-multiply two 64-bit ints using a 4-bit table (scan y once).
/// Complexity: up to 16 XORs and shifts; no nested nibble loops.
constexpr uint128_t ClMul(std::uint64_t x, std::uint64_t y) noexcept {
  const auto tbl = detail::ClMulNibbleTable(x);
  auto r = uint128_t{0};
  for (unsigned i=0; i!=16; ++i)
    r ^= tbl[(y>>(4*i)) & 0x0f] << (4*i);
  return r;
} // ClMul

template<std::uint64_t K>
constexpr uint128_t ClMulK(std::uint64_t y) noexcept {
  constexpr auto tbl = detail::ClMulNibbleTable(K);
  auto r = uint128_t{0};
  for (unsigned i = 0; i != 16; ++i)
    r ^= tbl[(y>>(4*i)) & 0x0f] << (4*i);
  return r;
} // ClMulK

} // tjg
