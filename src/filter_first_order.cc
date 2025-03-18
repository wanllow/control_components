#include "control/control_component/controller_task_base/common/include/filter_first_order.h"

namespace mfn {
namespace control {
bool FilterFirstOrder::SetFiltConst(const double& filter_weight_const) {
  if (filter_weight_const >= 0 && filter_weight_const <= 1) {
    filter_weight_const_ = filter_weight_const;
    return true;
  } else {
    return false;
    // caution: for this case, error log must be written.
  }
}
bool FilterFirstOrder::EnableLimit() {
  enable_limit_ = true;
  return enable_limit_;
}
bool FilterFirstOrder::DisableLimit() {
  enable_limit_ = false;
  return enable_limit_;
}
double FilterFirstOrder::Filt(double input_value) {
  double output_value = input_value * filter_weight_const_ +
                        output_value_ * (1 - filter_weight_const_);
  if (enable_limit_)
    output_value = std::max(lower_limit_, std::min(upper_limit_, output_value));
  output_value_ = output_value;
  return output_value_;
}
double FilterFirstOrder::Filt(double input_value, double filter_weight_const) {
  filter_weight_const_ =
      filter_weight_const;  // assign temporary parameter to permanent parameter

  return FilterFirstOrder::Filt(input_value);
}
}  // namespace control
}  // namespace mfn
