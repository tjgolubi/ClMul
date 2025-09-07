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

} // tjg
