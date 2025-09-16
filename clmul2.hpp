#include "../tjg32/tjg/Integer.hpp"

#include <concepts>
#include <utility>
#include <cstdint>

namespace tjg {

using uint128_t = unsigned __int128;

template<std::unsigned_integral T>
requires (sizeof(T) <= 4)
auto clmul1(T a, T b) -> uint_t<16*sizeof(T)>::least{
  using U = uint_t<16*sizeof(T)>::least;
  U r{0};
  while (b != 0) {
    r ^= a * (b & -b);
    b &= b - 1;
    r ^= a * (b & -b);
    b &= b - 1;
    r ^= a * (b & -b);
    b &= b - 1;
    r ^= a * (b & -b);
    b &= b - 1;
  }
  return r;
} // clmul1

constexpr uint128_t clmul2(std::uint64_t a, std::uint64_t b) noexcept {
  using std::uint64_t;
  using std::uint16_t;
  static constexpr unsigned B = 64;

  const auto msb = uint8_t(a >> (B-3));
  const auto a0 = a & (~uint64_t{0} >> 3);
  const auto a1 = a0 << 1;
  const auto a2 = a0 << 2;
  const auto a3 = a0 << 3;

  const uint64_t t[16] = {
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

  auto r = uint128_t{0};
  if (auto u = uint16_t(b); u != 0) {
    auto r0 = uint128_t{t[(u      ) & 0x0f]}      ;
    auto r1 = uint128_t{t[(u >>  4) & 0x0f]} <<  4;
    auto r2 = uint128_t{t[(u >>  8) & 0x0f]} <<  8;
    auto r3 = uint128_t{t[(u >> 12)       ]} << 12;
    r = r0 ^ r1 ^ r2 ^ r3;
  }

  if (auto u = uint16_t(b>>16); u != 0) {
    auto r0 = uint128_t{t[(u      ) & 0x0f]} << 16;
    auto r1 = uint128_t{t[(u >>  4) & 0x0f]} << 20;
    auto r2 = uint128_t{t[(u >>  8) & 0x0f]} << 24;
    auto r3 = uint128_t{t[(u >> 12)       ]} << 28;
    r ^= r0 ^ r1 ^ r2 ^ r3;
  }

  if (auto u = uint16_t(b>>32); u != 0) {
    auto r0 = uint128_t{t[(u      ) & 0x0f]} << 32;
    auto r1 = uint128_t{t[(u >>  4) & 0x0f]} << 36;
    auto r2 = uint128_t{t[(u >>  8) & 0x0f]} << 40;
    auto r3 = uint128_t{t[(u >> 12)       ]} << 44;
    r ^= r0 ^ r1 ^ r2 ^ r3;
  }

  if (auto u = uint16_t(b>>48); u != 0) {
    auto r0 = uint128_t{t[(u      ) & 0x0f]} << 48;
    auto r1 = uint128_t{t[(u >>  4) & 0x0f]} << 52;
    auto r2 = uint128_t{t[(u >>  8) & 0x0f]} << 56;
    auto r3 = uint128_t{t[(u >> 12)       ]} << 60;
    r ^= r0 ^ r1 ^ r2 ^ r3;
  }

  if (msb == 0)
    return r;

  auto r0 = uint128_t(b);
  if (msb & 1)
    r ^= r0 << (B-3);
  if (msb & 2)
    r ^= r0 << (B-2);
  if (msb & 4)
    r ^= r0 << (B-1);
  return r;
} // clmul2 64x64 -> 128

} // tjg
