#pragma once

#include <cmath>
#include <stdexcept>
#include <vector>
#include <numeric>

double IntegrateRectangular(const std::vector<double>& yvec, const double& dx,
                            const double& init_value);
double IntegrateRectangular(const std::vector<double>& yvec,
                            const std::vector<double>& xvec,
                            const double& init_value);
double IntegrateTrapezoidal(const std::vector<double>& yvec, const double& dx,
                            const double& init_value);
double IntegrateTrapezoidal(const std::vector<double>& yvec,
                            const std::vector<double>& xvec,
                            const double& init_value);
double IntegrateSimpson(const std::vector<double>& yvec, const double& dx,
                        const double& init_value);
double IntegrateSimpson(const std::vector<double>& yvec,
                        const std::vector<double>& xvec,
                        const double& init_value);
double IntegrateGauss(const std::vector<double>& yvec, const double& dx,
                      const double& init_value);
double IntegrateGauss(const std::vector<double>& yvec,
                      const std::vector<double>& xvec,
                      const double& init_value);