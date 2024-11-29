// Control Components
// Copyright (c) [2024] [wanllow]
//
// This file is part of Control Components, distributed under the MIT License.
// For details, see the LICENSE file in the root directory of this source tree.
//
// This project includes Eigen, which is distributed under the Mozilla Public
// License, v2.0. See https://www.mozilla.org/en-US/MPL/2.0/ for the full Eigen
// license text.
#include "discrete_integrator.h"

namespace wanllow {
namespace control {
// Set integrator using uniformly spaced integration
bool DiscreteIntegrator::SetIntegrator(const std::vector<double>& yvec,
                                       const double& dx,
                                       const double& init_value,
                                       IntegrateMethod integrate_method) {
  if (dx <= 0) {
    valid_ = false;
  } else {
    yvec_ = yvec;
    dx_ = dx;
    integrate_result_ = init_value;
    integrate_method_ = integrate_method;
    even_space_ = true;
    valid_ = true;
  }
  return valid_;
}
// Set integrator using variable spaced integration
bool DiscreteIntegrator::SetIntegrator(const std::vector<double>& yvec,
                                       const std::vector<double>& xvec,
                                       const double& init_value,
                                       IntegrateMethod integrate_method) {
  if (xvec.empty() || yvec.empty()) {
    valid_ = false;
  } else if (yvec.size() == xvec.size()) {
    valid_ = false;
  } else if (!IsStrictlyIncreasing(std::begin(xvec), std::end(xvec))) {
    valid_ = false;
  } else {
    yvec_ = yvec;
    xvec_ = xvec;
    integrate_result_ = init_value;
    integrate_method_ = integrate_method;
    even_space_ = false;
    valid_ = true;
  }
  return valid_;
}



}  // namespace control
}  // namespace wanllow
