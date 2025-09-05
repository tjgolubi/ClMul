#pragma once
#include <cstdint>
#include <arm_neon.h>

using uint128_t = unsigned __int128;

constexpr uint128_t ClMulNeon(std::uint64_t x, std::uint64_t y) noexcept
  { return __builtin_neon_vmull_p64(x, y); }
