#pragma once
#include <cstdint>
#include <utility>

namespace tjg {

using uint128_t = unsigned __int128;

/// Multiply two 64-bit unsigned integers using carryless multiplication.
/// The result is a 128-bit XOR-accumulated product over GF(2).
constexpr uint128_t ClMulBit(std::uint64_t x, std::uint64_t y) noexcept {
  auto r = uint128_t{0};
  for (auto a = uint128_t{x}; y != 0; y>>=1, a<<=1) {
    if (y & 1)
      r ^= a;
  }
  return r;
} // ClMulBit

#if 0
/// Multiply two 64-bit unsigned integers using carryless multiplication.
/// The result is a 128-bit XOR-accumulated product over GF(2).
constexpr uint128_t ClMulBit(std::uint64_t x, std::uint64_t y) noexcept {
  auto r = uint128_t{0};
  auto a = uint128_t{x};
  for (unsigned i=0; i!=64; ++i) {
    if ((y>>i) & 1)
      r ^= (a<<i);
  }
  return r;
} // ClMulBit
#endif

} // tjg
