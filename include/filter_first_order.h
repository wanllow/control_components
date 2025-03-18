#pragma once
#include <algorithm>  // 添加std::max需要
#include <stdexcept>  // 添加异常处理需要
#include "common_utils.h"

namespace wanllow {
namespace control {
/**
 * @brief 一阶滤波器类，提供基本滤波功能
 * 
 * 实现一阶低通滤波算法，支持动态调整滤波系数
 */
class FilterFirstOrder {
 public:
  /// @brief 默认构造函数，使用默认滤波系数0.5
  FilterFirstOrder() = default;
  
  /// @brief 带初始滤波系数的构造函数
  /// @param filter_weight_const 初始滤波系数，超出[0,1]范围将被自动限制
  explicit FilterFirstOrder(const double& filter_weight_const) 
      : filter_weight_const_(
          std::max(0.0,  // 确保不小于0
            std::min(filter_weight_const, 1.0))  // 确保不大于1
        ) {}
  ~FilterFirstOrder(){};
  bool SetFiltConst(const double& filter_weight_const);
  bool EnableLimit();
  bool DisableLimit();
  double Filt(const double& input_value);
  double Filt(const double& input_value, const double& filter_weight_const);

 protected:
  

 private:
  double output_value_{0};
  double filter_weight_const_{0.5};
  double upper_limit_{0};
  double lower_limit_{0};
  bool enable_limit_{false};
};
}  // namespace control
}  // namespace wanllow
