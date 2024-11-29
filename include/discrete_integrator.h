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

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <vector>
#include <stdexcept>
#include "common_utils.h"

namespace wanllow {
namespace control {
enum class IntegrateMethod {
  hold = 0,         // hold means stop integration
  rectangular = 1,  // just weighted accumulation
  trapezoidal = 2,  // linear interpolation assumption
  simpson = 3,      // square spline assumption
  gauss = 4         // Gauss method
};
class DiscreteIntegrator {
 public:
  // Constructors and destructor
  DiscreteIntegrator() = default;
  // Construct fix-step integrator
  DiscreteIntegrator(
      const std::vector<double>& yvec, const double& dx,
      const double& init_value = 0,
      IntegrateMethod integrate_method = IntegrateMethod::trapezoidal)
      : yvec_{yvec},
        dx_{dx},
        integrate_result_{init_value},
        integrate_method_{integrate_method},
        even_space_{true},
        reset_value_{init_value} {
    xvec_.clear();
    if (dx > 0) {
      valid_ = true;
    } else {
      valid_ = false;
    }
  }
  // Construct variable step integrator
  DiscreteIntegrator(
      const std::vector<double>& yvec, const std::vector<double>& xvec,
      const double& init_value = 0,
      IntegrateMethod integrate_method = IntegrateMethod::trapezoidal) {
    SetIntegrator(yvec, xvec, init_value, integrate_method);
  }

  ~DiscreteIntegrator() = default;
  // Setting data to integrator, input parameter is checked before assignment
  bool SetIntegrator(
      const std::vector<double>& yvec, const double& dx,
      const double& init_value = 0,
      IntegrateMethod integrate_method = IntegrateMethod::trapezoidal);
  bool SetIntegrator(
      const std::vector<double>& yvec, const std::vector<double>& xvec,
      const double& init_value = 0,
      IntegrateMethod integrate_method = IntegrateMethod::trapezoidal);
  // Setting status
  inline void Enable(IntegrateMethod method) { integrate_method_ = method; }
  inline void Disable() { integrate_method_ = IntegrateMethod::hold; }
  inline void Reset() { integrate_result_ = reset_value_; }
  inline void Reset(const double& reset_value) {
    integrate_result_ = reset_value;
    reset_value_ = reset_value;
  }
  // Integrate functions
  double Integrate();

 private:
  std::vector<double> yvec_{};  // initial state of y vector is empty
  std::vector<double> xvec_{};  // initial state of x vector is empty
  double dx_{0};                // initial state of dx is 0
  double integrate_result_{0};  // initial state of integrate result is 0
  double reset_value_{0};       // reset value
  bool even_space_{
      false};          // the flag represents this is fixed distance integration
  bool valid_{false};  // flag of valid
  IntegrateMethod integrate_method_{IntegrateMethod::trapezoidal};  // method
};
}  // namespace control
}  // namespace wanllow
