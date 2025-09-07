#include "ClMulNeon.hpp"
#include "ClMul.hpp"

#include <random>
#include <chrono>
#include <functional>
#include <map>
#include <iostream>
#include <iomanip>
#include <bit>
#include <cstdint>

constexpr int TestCount = 100;
constexpr int LoopCount = 1 << 20;

using tjg::uint128_t;

using Clock = std::chrono::steady_clock;

#if 0
template<std::unsigned_integral U>
struct CoutType: public std::conditional<(sizeof(U) == 1), unsigned, U> { };

template<std::unsigned_integral U>
auto Value(U x) -> typename CoutType<U>::type
  { return static_cast<typename CoutType<U>::type>(x); }
#endif

constexpr auto DefaultSeed = std::uint64_t{54321};

auto RandEng = std::mt19937_64{DefaultSeed};

void RandSeed(std::uint64_t seed = DefaultSeed) noexcept {
  RandEng.seed(seed);
} // RandSeed

std::uint64_t Rand64() noexcept {
  static auto dist = std::uniform_int_distribution<std::uint64_t>
                                {0, std::numeric_limits<std::uint64_t>::max()};
  return dist(RandEng);
} // Rand64

inline std::vector<std::uint64_t> MakeRandVec() noexcept {
  std::vector<std::uint64_t> v;
  v.reserve(2 * LoopCount);
  for (auto i = 2*LoopCount; i--; )
    v.push_back(Rand64());
  return v;
}

const auto RandVec = MakeRandVec();
std::size_t RandNext = 0;

void RandReset() noexcept { RandNext = 0; }

std::uint64_t Rand() noexcept { return RandVec[RandNext++]; }

std::ostream& operator<<(std::ostream& os, uint128_t x) {
  using namespace std;
  constexpr auto w = 2 * sizeof(uint64_t);
  auto c = os.fill('0');
  os << "0x" << setw(w) << uint64_t(x>>64) << setw(w) << uint64_t(x);
  os.fill(c);
  return os;
}

// using Fn2 = uint128_t (*)(std::uint64_t, std::uint64_t);
// using Fn1 = uint128_t (*)(std::uint64_t);

// using FnK = std::function<uint128_t(std::uint64_t)>;
constexpr auto K = std::uint64_t{0xfedcba987654321f};

std::map<std::string, Clock::duration> Times;

template<class Fn2>
uint128_t Test(std::string name, Fn2 fn) {
  uint128_t result = 0;
  RandReset();
  auto start = Clock::now();
  for (int i = 0; i != LoopCount; ++i) {
    auto x = Rand();
    auto y = Rand();
    result ^= fn(x, y);
  }
  auto stop = Clock::now();
  Times[name] += (stop - start);
  return result;
} // Test

template<class Fn2>
uint128_t TestK1(std::string name, Fn2 fn) {
  uint128_t result = 0;
  RandReset();
  auto start = Clock::now();
  for (int i = 0; i != LoopCount; ++i) {
    auto x = Rand() ^ Rand();
    result ^= fn(K, x);
  }
  auto stop = Clock::now();
  Times[name] += (stop - start);
  return result;
} // TestK1

template<class Fn2>
uint128_t TestK2(std::string name, Fn2 fn) {
  uint128_t result = 0;
  RandReset();
  auto start = Clock::now();
  for (int i = 0; i != LoopCount; ++i) {
    auto x = Rand() ^ Rand();
    result ^= fn(x, K);
  }
  auto stop = Clock::now();
  Times[name] += (stop - start);
  return result;
} // TestK2

template<class Fn1>
uint128_t Test1(std::string name, Fn1 fn) {
  uint128_t result = 0;
  RandReset();
  auto start = Clock::now();
  for (int i = 0; i != LoopCount; ++i) {
    auto x = Rand() ^ Rand();
    result ^= fn(x);
  }
  auto stop = Clock::now();
  Times[name] += (stop - start);
  return result;
} // Test1

int main() {
  using namespace std;
  using namespace tjg;

  auto res_2 = uint128_t{0};
  auto res_k = uint128_t{0};
  for (int i=0; i!=TestCount; ++i) {
    res_2^= Test  ("Neon  2-arg",   ClMulNeon);
    res_2^= Test  ("ClMul 2-arg",   ClMul);
    res_k^= TestK1("Neon  K_left",  ClMulNeon);
    res_k^= TestK2("Neon  K_right", ClMulNeon);
    res_k^= TestK1("ClMul_K_left",  ClMul);
    res_k^= TestK2("ClMul_K_right", ClMul);
    res_k^= Test1 ("ClMulK1",       ClMulK<K>);
    res_k^= Test1 ("ClMulK2",       ClMulK<K>);
  }

  cout << fixed << setprecision(6);
  for (const auto& [name, dt]: Times) {
    auto s = std::chrono::duration<double>{dt};
    cout << left << setw(14) << name << right << ' ' << setw(8) << s << '\n';
  }

  cout << "res_2 = " << res_2 << " res_k = " << res_k << endl;

  return EXIT_SUCCESS;
} // main
