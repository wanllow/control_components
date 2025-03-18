#include "integrate_functions.h"

// Implementation of rectangular integration with a constant step size (dx)
double IntegrateRectangular(const std::vector<double>& yvec, const double& dx,
                            const double& init_value) {
  // Check for valid input: yvec must have at least two elements, and dx must be
  // positive
  if (yvec.empty()) {
    return init_value;
  } else {
    // Calculate the sum of y values from yvec[0] to yvec[n-2]
    double sum = std::accumulate(yvec.begin(), yvec.end() - 1, 0.0);

    // Multiply the sum by dx and add the initial value to get the final result
    return sum * dx + init_value;
  }
}

// Implementation of rectangular integration with variable step sizes (xvec)
double IntegrateRectangular(const std::vector<double>& yvec,
                            const std::vector<double>& xvec,
                            const double& init_value) {
  // Check for valid input: yvec and xvec must be the same size and yvec must
  // contain at least two elements
  if (yvec.empty()) {
    return init_value;
  } else if (yvec.size() != xvec.size()) {
    throw std::invalid_argument(
        "Invalid input: yvec must have the same size as xvec");
  } else {
    double result = init_value;  // Initialize the result with the initial value

    // Iterate through each value of yvec up to the second-to-last element
    for (size_t index = 0; index < yvec.size() - 1; ++index) {
      // Accumulate the height of each rectangle (y value) into the result
      result += yvec[index] * (xvec[index + 1] - xvec[index]);
    }
    return result;
  }
}

// Implementation of trapezoidal integration with a constant step size (dx)
double IntegrateTrapezoidal(const std::vector<double>& yvec, const double& dx,
                            const double& init_value) {
  // Check for valid input: yvec must not be empty and dx must be positive
  if (yvec.empty()) {
    return init_value;
  } else {
    // Calculate the sum of y values, adjusting for the trapezoidal method
    // We subtract half the first and last values because they only contribute
    // to one of the trapezoids' areas
    double sum = std::accumulate(yvec.begin(), yvec.end(), 0.0) -
                 (yvec.front() + yvec.back()) / 2;

    // Multiply the summed area by dx and add the initial value
    return sum * dx + init_value;
  }
}

// Implementation of trapezoidal integration with variable step sizes (xvec)
double IntegrateTrapezoidal(const std::vector<double>& yvec,
                            const std::vector<double>& xvec,
                            const double& init_value) {
  // Check for valid input: yvec and xvec must be the same size and yvec must
  // not be empty
  if (yvec.empty()) {
    return init_value;
  } else if (yvec.size() != xvec.size()) {
    throw std::invalid_argument(
        "Invalid input: yvec must have the same size as xvec");
  } else {
    double result = init_value;  // Initialize the result with the initial value

    // Using the trapezoidal area formula: A = (a + b) * h / 2
    // Iterate through each interval defined by yvec and xvec
    for (size_t index = 0; index < yvec.size() - 1; ++index) {
      // Update the result with the area of the trapezoid
      result += (yvec[index] + yvec[index + 1]) *
                (xvec[index + 1] - xvec[index]) / 2;
    }
    return result;
  }
}

// Implementation of Simpson integration with constant step size (dx)
double IntegrateSimpson(const std::vector<double>& yvec, const double& dx,
                        const double& init_value) {
  const int integrate_array_size = yvec.size();
  if (yvec.empty()) {
    return init_value;
  }
  // Handle base cases where size is 1 or 2
  // Handle base cases where size is 1 or 2
  else if (integrate_array_size < 3) {
    // Approximate integration as a rectangle for a single point
    return IntegrateTrapezoidal(yvec, dx, init_value);
  } else {
    // If size is odd, apply the standard Simpson's rule for all points
    if (integrate_array_size & 0x1 == 1) {
      double sum_odd = 0;
      double sum_even = 0;
      for (size_t index = 1; index != integrate_array_size - 1; ++index) {
        if (index & 0x1) {
          sum_odd += yvec[index];
        } else {
          sum_even += yvec[index];
        }
      }

      // Calculate the integral using Simpson's rule
      return (yvec.front() + sum_odd * 4 + sum_even * 2 + yvec.back()) * dx /
                 3.0 +
             init_value;
  }

  // If size is even, apply Simpson's rule to the first (size-1) elements and
    // recursively call IntegrateSimpson for the last two points
    if (integrate_array_size > 3 && (integrate_array_size & 0x1) == 0) {
      return IntegrateSimpson(std::vector<double>(yvec.begin(), yvec.end() - 1),
                              dx, init_value) +
             ((yvec[integrate_array_size - 2] + yvec.back()) * dx / 2.0);
    }
  }
  // Fallback return for unexpected cases
  return init_value;
} // %end of function definition: IntegrateSimpson

// Implementation of Simpson integration with variable step sizes (xvec)
double IntegrateSimpson(const std::vector<double>& yvec,
                        const std::vector<double>& xvec,
                        const double& init_value) {
  // Simpson's method has no advantage in uneven interval integration, use
  // trapezoidal method instead.
  return IntegrateTrapezoidal(yvec, xvec, init_value);
}

// Implementation of Gauss integration with a constant step size (dx)
double IntegrateGauss(const std::vector<double>& yvec, const double& dx,
                      const double& init_value) {
  // Check for valid input: yvec must have at least two elements, and dx must be positive
  if (yvec.empty() || dx <= 0.0) {
    return init_value;
  }
  
  // Weights and nodes for 2-point Gaussian Quadrature
  const double weights[2] = {1.0, 1.0};
  const double nodes[2] = {-1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};
  
  double result = init_value;

  // Integrate over the intervals defined by dx
  for (size_t index = 0; index < yvec.size() - 1; ++index) {
    // Interval bounds
    double x0 = index * dx;
    double x1 = (index + 1) * dx;

    // Midpoint and half-width of the interval
    double xm = (x0 + x1) / 2.0;
    double half_width = (x1 - x0) / 2.0;

    // Map nodes to the interval
    double integral = 0.0;
    for (int i = 0; i < 2; ++i) {
      double xi = xm + nodes[i] * half_width;
      // Approximate f(xi) as a linear interpolation between y[index] and y[index+1]
      double yi = yvec[index] + (yvec[index + 1] - yvec[index]) * (xi - x0) / (x1 - x0);
      integral += weights[i] * yi;
    }

    // Scale by the interval width
    result += integral * half_width;
  }
  return result;
}

// Implementation of Gauss integration with variable step sizes (xvec)
double IntegrateGauss(const std::vector<double>& yvec,
                      const std::vector<double>& xvec,
                      const double& init_value) {
  // Check for valid input: yvec and xvec must have the same size and contain at least two elements
  if (yvec.empty() || yvec.size() != xvec.size()) {
    throw std::invalid_argument("Invalid input: yvec and xvec must be of the same size");
  }

  // Weights and nodes for 2-point Gaussian Quadrature
  const double weights[2] = {1.0, 1.0};
  const double nodes[2] = {-1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};

  double result = init_value;

  // Integrate over the variable intervals defined by xvec
  for (size_t index = 0; index < yvec.size() - 1; ++index) {
    // Interval bounds
    double x0 = xvec[index];
    double x1 = xvec[index + 1];

    // Midpoint and half-width of the interval
    double xm = (x0 + x1) / 2.0;
    double half_width = (x1 - x0) / 2.0;

    // Map nodes to the interval
    double integral = 0.0;
    for (int i = 0; i < 2; ++i) {
      double xi = xm + nodes[i] * half_width;
      // Approximate f(xi) as a linear interpolation between y[index] and y[index+1]
      double yi = yvec[index] + (yvec[index + 1] - yvec[index]) * (xi - x0) / (x1 - x0);
      integral += weights[i] * yi;
    }

    // Scale by the interval width
    result += integral * half_width;
  }
  return result;
}
