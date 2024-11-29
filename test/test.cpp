#include "test.h"

static const int points_num = 50;

void TestMultiPoints() {
  try {
    // Test cases
    std::vector<std::vector<double>> test_cases = {
        {},                              // Length 0
        {1.0},                           // Length 1
        {1.0, 2.0},                      // Length 2
        {1.0, 4.0, 1.0},                 // Length 3
        {1.0, 4.0, 1.0, 2.0},            // Length 4
        {1.0, 4.0, 1.0, 2.0, 3.0},       // Length 5
        {1.0, 4.0, 1.0, 2.0, 3.0, 5.0},  // Length 6
    };

    // Add a longer test case
    std::vector<double> long_test_case(100, 1.0);
    // Length 100, all values are 1.0
    double dx = 1.0;          // Fixed step size
    double init_value = 0.0;  // Initial integration value

    // Test each case
    for (size_t i = 0; i < test_cases.size(); ++i) {
      try {
        std::cout << "Test Case " << i << " (Length " << test_cases[i].size()
                  << "): " << IntegrateSimpson(test_cases[i], dx, init_value)
                  << "\n";
      } catch (const std::exception& e) {
        std::cout << "Test Case " << i << " (Length " << test_cases[i].size()
                  << ") Failed: " << e.what() << "\n";
      }
    }

    // Test long case
    std::cout << "Test Case Long (Length " << long_test_case.size()
              << "): " << IntegrateSimpson(long_test_case, dx, init_value)
              << "\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}

void TestSineIntegration() {
  const double dx =
      M_PI_2 / points_num;  // Step size (π/2 divided by number of intervals)
  std::vector<double> yvec(points_num + 1);  // Array of y-values for sin(x)
  std::vector<double> xvec(points_num + 1);  // Array of x-values for sin(x)

  // Fill yvec with sin(x) values
  for (int i = 0; i <= points_num; ++i) {
    xvec[i] = i * dx;
    yvec[i] = std::sin(xvec[i]);    
  }
  std::cout << "Exact value is: " << 1 << std::endl;
  try {
    double result = IntegrateRectangular(yvec, dx, 0.0);
    std::cout << "Rectangular Integration of sin(x) from 0 to pi/2 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateRectangular(yvec, xvec, 0.0);
    std::cout << "Rectangular Integration of sin(x) from 0 to pi/2 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateTrapezoidal(yvec, dx, 0.0);
    std::cout << "Trapezoidal Integration of sin(x) from 0 to pi/2 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateTrapezoidal(yvec, xvec, 0.0);
    std::cout << "Trapezoidal Integration of sin(x) from 0 to pi/2 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateSimpson(yvec, dx, 0.0);
    std::cout << "Simpson Integration of sin(x) from 0 to pi/2 with " << points_num
              << " intervals: " << result << std::endl;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Test Failed: " << e.what() << std::endl;
  }
}

void TestExpIntegration() {
  const double dx = 1.0 / points_num;  // Step size (1 divided by number of intervals)
  std::vector<double> yvec(points_num + 1);  // Array of y-values for sin(x)
  std::vector<double> xvec(points_num + 1);  // Array of x-values for sin(x)

  // Fill yvec with exp(x) values
  for (int i = 0; i <= points_num; ++i) {
    xvec[i] = i * dx;
    yvec[i] = std::exp(xvec[i]);
  }
  std::cout << "Exact value of is: " << std::exp(1) - 1 << std::endl;
  try {
    double result = IntegrateRectangular(yvec, dx, 0.0);
    std::cout << "Rectangular Integration of exp(x) from 0 to 1 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateRectangular(yvec, xvec, 0.0);
    std::cout << "Rectangular Integration of exp(x) from 0 to 1 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateTrapezoidal(yvec, dx, 0.0);
    std::cout << "Trapezoidal Integration of exp(x) from 0 to 1 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateTrapezoidal(yvec, xvec, 0.0);
    std::cout << "Trapezoidal Integration of exp(x) from 0 to 1 with " << points_num
              << " intervals: " << result << std::endl;
    result = IntegrateSimpson(yvec, dx, 0.0);
    std::cout << "Simpson Integration of exp(x) from 0 to 1 with " << points_num
              << " intervals: " << result << std::endl;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Test Failed: " << e.what() << std::endl;
  }
}