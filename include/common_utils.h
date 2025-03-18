#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>
#include <Eigen/Core>
#include "common/platform_log.h"

namespace mfn {
namespace control {
namespace algorithm {
// 泛型函数：约等于，可接收不同的浮点数作为输入量，支持:float, double, long
// double
template <typename T1, typename T2, typename T3>
typename std::enable_if<std::is_floating_point<T1>::value &&
                            std::is_floating_point<T2>::value &&
                            std::is_floating_point<T3>::value,
                        bool>::type
ApproxEqual(const T1& value1, const T2& value2, const T3& tolerance) {
  // 避免输入意外
  if (std::isnan(value1) || std::isnan(value2)) {
    return false;
  }
  if (std::isinf(value1) || std::isinf(value2)) {
    return value1 == value2;
  }

  // 绝对误差快速判断
  return std::abs(value1 - value2) <=
         ((std::isnan(tolerance) || std::isinf(tolerance))
              ? std::numeric_limits<T3>::epsilon()
              : tolerance);
}

template <typename T1, typename T2>
typename std::enable_if<std::is_floating_point<T1>::value &&
                            std::is_floating_point<T2>::value,
                        bool>::type
ApproxEqual(const T1& value1, const T2& value2) {
  //
  return ApproxEqual(
      value1, value2,
      std::numeric_limits<typename std::common_type<T1, T2>::type>::epsilon());
}

// 泛型clamp
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
T clamp(const T& value, const T& lower, const T& upper) {
  if (upper < lower) {
    SERROR << "Invalid clamp range [upper:" << upper 
    << " < lower:" << lower << "] at " << __FILE__ << ":" << __LINE__; // [^3]
    const T& lower_limit = upper;
    const T& upper_limit = lower;
  } else {
    const T& lower_limit = lower;
    const T& upper_limit = upper;
  }
  return value < lower_limit   ? lower_limit
         : value > upper_limit ? upper_limit
                               : value;
}

// 通用迭代器版本判断单调递增
template <typename Iterator>
bool IsStrictlyIncreasingImpl(Iterator begin, Iterator end) {
  if (begin == end || std::next(begin) == end) return true;

  for (auto it = begin; std::next(it) != end; ++it) {
    if (*it >= *std::next(it)) {
      return false;
    }
  }
  return true;
}
}  // namespace algorithm

// 标准容器版本（支持std::vector/std::array等）
template <typename Container>
auto IsStrictlyIncreasing(const Container& c) -> typename std::enable_if<
    std::is_arithmetic<typename Container::value_type>::value &&
        !std::is_base_of<Eigen::EigenBase<Container>, Container>::value,
    bool>::type {
  using std::begin;
  using std::end;
  return algorithm::IsStrictlyIncreasingImpl(begin(c), end(c));
}

// 原始数组版本（支持T[]）
template <typename T, size_t N>
auto IsStrictlyIncreasing(const T (&arr)[N]) ->
    typename std::enable_if<std::is_arithmetic<T>::value, bool>::type {
  return algorithm::IsStrictlyIncreasingImpl(arr, arr + N);
}

// Eigen专用版本（支持Matrix/Array）
template <typename Derived>
auto IsStrictlyIncreasing(const Eigen::EigenBase<Derived>& matrix) ->
    typename std::enable_if<std::is_arithmetic<typename Derived::Scalar>::value,
                            bool>::type {
  const auto& derived = matrix.derived();
  if (derived.size() <= 1) return true;

  // 将Eigen对象视为连续内存访问
  const auto* data = derived.data();
  return algorithm::IsStrictlyIncreasingImpl(data, data + derived.size());
}

}  // namespace control
}  // namespace mfn