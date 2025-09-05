#include "ClMulNeon.hpp"

#include <random>
#include <chrono>
#include <map>
#include <iostream>
#include <iomanip>
#include <bit>
#include <cstdint>

// constexpr int TestCount = 100;
constexpr int LoopCount = 1 << 18;

using Clock = std::chrono::steady_clock;

#if 0
template<std::unsigned_integral U>
struct CoutType: public std::conditional<(sizeof(U) == 1), unsigned, U> { };

template<std::unsigned_integral U>
auto Value(U x) -> typename CoutType<U>::type
  { return static_cast<typename CoutType<U>::type>(x); }
#endif

constexpr auto DefaultSeed = std::uint64_t{12345};

auto RandEng = std::mt19937_64{DefaultSeed};

void RandSeed(std::uint64_t seed = DefaultSeed) {
  RandEng.seed(seed);
} // RandSeed

std::uint64_t Rand64() {
  auto dist = std::uniform_int_distribution<std::uint64_t>
                                {0, std::numeric_limits<std::uint64_t>::max()};
  return dist(RandEng);
} // Rand64

std::ostream& operator<<(std::ostream& os, uint128_t x) {
  using namespace std;
  constexpr auto w = 2 * sizeof(uint64_t);
  auto c = os.fill('0');
  os << "0x" << setw(w) << uint64_t(x>>64) << setw(w) << uint64_t(x);
  os.fill(c);
  return os;
}

using Fn = uint128_t (*)(std::uint64_t, std::uint64_t);

std::map<std::string, Clock::duration> Times;

int Test(std::string name, Fn fn) {
  auto fails = 0;
  auto start = Clock::now();
  for (int i = 0; i != LoopCount; ++i) {
    auto x = Rand64();
    auto y = Rand64();
    auto r1 = fn(x, y);
    fails += (r1 != ClMulNeon(x, y));
  }
  auto stop = Clock::now();
  Times[name] += (stop - start);
  return fails;
} // Test

int main() {
  using namespace std;

  auto x = Rand64();
  auto y = Rand64();
  auto r1 = ClMulNeon(x, y);
  auto x2 = std::byteswap(x);
  auto y2 = y;
  auto r2 = std::byteswap(ClMulNeon(x2, y2));
  auto w = 2 * sizeof(std::uint64_t);
  cout << setfill('0') << hex
    << "x=0x" << setw(w) << x << " y=0x" << setw(w) << y
    << "\n  r1=" << r1 << ' ' << std::byteswap(r1)
    << "\n  r2=" << r2 << ' ' << std::byteswap(r2)
    << '\n';

  return EXIT_SUCCESS;
} // main
