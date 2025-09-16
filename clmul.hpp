#pragma once

#include <array>
#include <concepts>
#include <cstdint>

#if __has_include(<arm_neon.h>)
#include <arm_neon.h>
#define TJG_CLMUL_BUILTIN NEON
#elif __has_include(<immintrin.h>)
#include <immintrin.h>
#define TJG_CLMUL_BUILTIN PCLMUL
#else
#undef TJG_CLMUL_BUILTIN
#endif

namespace tjg {

#ifdef __SIZEOF_INT128__
using uint128_t = unsigned __int128;
#endif

#if !defined(__SIZEOF_INT128__) || defined(TJG_CLMUL_EXTRA)

namespace detail {

template<std::integral T> struct ClmulResult { using type = void; };
template<> struct ClmulResult<std::uint8_t > { using type = std::uint16_t; };
template<> struct ClmulResult<std::uint16_t> { using type = std::uint32_t; };
#ifdef UINT64_MAX
template<> struct ClmulResult<std::uint32_t> { using type = std::uint64_t; };
#ifdef __SIZEOF_INT128__
template<> struct ClmulResult<std::uint64_t> { using type = uint128_t;     };
#endif
#endif

} // detail

template<std::unsigned_integral T>
constexpr auto clmulTiny(T a, T b) noexcept {
  using U = detail::ClmulResult<T>::type;
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
} // clmulTiny

#endif

#if !defined(__SIZEOF_INT128__)

template<std::unsigned_integral T>
constexpr auto clmul(T a, T b) noexcept { return clmulTiny(a, b); }

template<std::integral T, T K>
constexpr auto clmulK(T x) noexcept { return clmulTiny(a, K); }

#else

#if !defined(TJG_CLMUL_BUILTIN) || defined(TJG_CLMUL_EXTRA)

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
constexpr uint128_t clmulTab(std::uint64_t x, std::uint64_t y) noexcept {
  const auto tbl = detail::ClMulNibbleTable(x);
  auto r = uint128_t{0};
  for (unsigned i=0; i!=16; ++i)
    r ^= tbl[(y>>(4*i)) & 0x0f] << (4*i);
  return r;
} // clmulTab

template<std::uint64_t K>
constexpr uint128_t clmulKTab(std::uint64_t y) noexcept {
  constexpr auto tbl = detail::ClMulNibbleTable(K);
  auto r = uint128_t{0};
  for (unsigned i = 0; i != 16; ++i)
    r ^= tbl[(y>>(4*i)) & 0x0f] << (4*i);
  return r;
} // clmulKTab

/// Multiply two 64-bit unsigned integers using carryless multiplication.
/// The result is a 128-bit XOR-accumulated product over GF(2).
constexpr uint128_t clmulBit(std::uint64_t x, std::uint64_t y) noexcept {
  auto r = uint128_t{0};
  if (x < y)
    std::swap(x, y);
  for (auto a = uint128_t{x}; y != 0; y>>=1, a<<=1) {
    if (y & 1)
      r ^= a;
  }
  return r;
} // clmulBit

/// Multiply two 64-bit unsigned integers using carryless multiplication.
/// The result is a 128-bit XOR-accumulated product over GF(2).
constexpr uint128_t clmulBit2(std::uint64_t x, std::uint64_t y) noexcept {
  auto r = uint128_t{0};
  auto a = uint128_t{x};
  for (unsigned i=0; i!=64; ++i) {
    if ((y>>i) & 1)
      r ^= (a<<i);
  }
  return r;
} // clmulBit2

#endif

#if !defined(TJG_CLMUL_BUILTIN)

constexpr uint128_t clmul(std::uint64_t x, std::uint64_t y) noexcept
  { return clmulTab(x, y); }

template<std::uint64_t K>
constexpr uint128_t clmulK(std::uint64_t x) noexcept
  { return clmulKTab<K>(x); }

#elif TJG_CLMUL_BUILTIN == NEON

constexpr uint128_t clmul(std::uint64_t x, std::uint64_t y) noexcept
  { return __builtin_neon_vmull_p64(x, y); }

template<std::uint64_t K>
constexpr uint128_t clmulK(std::uint64_t x) noexcept
  { return clmul(x, K); }

#elif TJG_CLMUL_BUILTIN == PCLMUL

constexpr uint128_t clmul(std::uint64_t x, std::uint64_t y) noexcept {
  auto vx = _mm_set_epi64x(x, y);
  auto vz = _mm_clmulepi64_si128(vx, vx, 0x01);
  return std::bit_cast<uint128_t>(vz);
}

template<std::uint64_t K>
constexpr uint128_t clmulK(std::uint64_t x) noexcept
  { return clmul(x, K); }

#else

#error "TJG_CLMUL_BUILTIN is invalid: " TJG_CLMUL_BUILTIN

#endif // TJG_CLMUL_BUILTIN

#endif // __SIZE_OF_INT128__

} // tjg
