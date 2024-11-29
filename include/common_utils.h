// Control Components
// Copyright (c) [2024] [wanllow]
//
// This file is part of Control Components, distributed under the MIT License.
// For details, see the LICENSE file in the root directory of this source tree.
//
// This project includes Eigen, which is distributed under the Mozilla Public
// License, v2.0. See https://www.mozilla.org/en-US/MPL/2.0/ for the full Eigen
// license text.

#pragma once
#include <limits>
#include <algorithm>
#include <array>
#include <vector>
#include <type_traits>
#include <cassert>


namespace wanllow {
namespace control {
// double epsilon = std::numeric_limits<double>::epsilon();
template <typename Iterator>
typename std::enable_if<std::is_arithmetic<typename std::iterator_traits<Iterator>::value_type>::value, bool>::type
IsStrictlyIncreasing(Iterator begin, Iterator end) {
    if (begin == end || std::next(begin) == end) {
        return true;  // Empty or single-element range is considered strictly increasing
    }
    for (auto it = begin; std::next(it) != end; ++it) {
        if (*it >= *std::next(it)) {
            return false;
        }
    }
    return true;
}

// Overload for raw arrays
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type
IsStrictlyIncreasing(T* begin, T* end) {
    if (begin == end || std::next(begin) == end) {
        return true;  // Empty or single-element range is considered strictly increasing
    }

    for (auto it = begin; std::next(it) != end; ++it) {
        if (*it >= *std::next(it)) {
            return false;
        }
    }
    return true;
}



}  // namespace control
}  // namespace wanllow