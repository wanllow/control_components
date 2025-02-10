#include "test_common_utils.h"

namespace wanllow {
namespace test {
namespace detail {
    // 通用迭代器版本实现
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
} // namespace detail

// 标准容器版本（支持std::vector/std::array等）
template <typename Container>
auto IsStrictlyIncreasing(const Container& c) -> typename std::enable_if<
    std::is_arithmetic<typename Container::value_type>::value &&
    !std::is_base_of<Eigen::EigenBase<Container>, Container>::value,
    bool
>::type {
    using std::begin;
    using std::end;
    return detail::IsStrictlyIncreasingImpl(begin(c), end(c));
}

// 原始数组版本（支持T[]）
template <typename T, size_t N>
auto IsStrictlyIncreasing(const T (&arr)[N]) -> typename std::enable_if<
    std::is_arithmetic<T>::value, bool
>::type {
    return detail::IsStrictlyIncreasingImpl(arr, arr + N);
}

// Eigen专用版本（支持Matrix/Array）
template <typename Derived>
auto IsStrictlyIncreasing(const Eigen::EigenBase<Derived>& matrix) -> typename std::enable_if<
    std::is_arithmetic<typename Derived::Scalar>::value, bool
>::type {
    const auto& derived = matrix.derived();
    if (derived.size() <= 1) return true;
    
    // 将Eigen对象视为连续内存访问
    const auto* data = derived.data();
    return detail::IsStrictlyIncreasingImpl(data, data + derived.size());
}

} // namespace test
} // namespace wanllow