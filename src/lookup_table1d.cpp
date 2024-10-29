#include "lookup_table1d.h"

// Assign table data with new input values, validate first.
LookupTable::AssignmentState LookupTable1D::AssignTableData(const Eigen::RowVectorXd &x_axis, const Eigen::RowVectorXd &y_table)
{
    if (CheckTableState(x_axis, y_table) == TableState::valid)
    {
        x_axis_ = x_axis;
        y_table_ = y_table;
        bool refresh = RefreshTableState(); // redundant check, and refresh state in table
        return table_valid_ ? AssignmentState::success : AssignmentState::fail;
    }
    else
    {
        // if the input value is invalid, check the current table value to set the table_state flag
        bool refresh = RefreshTableState();
        return table_valid_ ? AssignmentState::remain : AssignmentState::fail;
    }
}
LookupTable::AssignmentState LookupTable1D::AssignTableData(const std::vector<double> &x_vec, const std::vector<double> &y_vec)
{
    // this is vector edition, first convert then call the base AssignTableData function.
    Eigen::RowVectorXd x_axis = Eigen::Map<const Eigen::RowVectorXd>(x_vec.data(), x_vec.size());
    Eigen::RowVectorXd y_table = Eigen::Map<const Eigen::RowVectorXd>(y_vec.data(), y_vec.size());
    return AssignTableData(x_axis, y_table);
}
// AssignTableData is related with three functions: CheckTableState, RefreshTableState, ClearTable.
inline bool LookupTable1D::ClearTable()
{
    x_axis_.resize(0);
    y_table_.resize(0);
    table_valid_ = false;
    table_empty_ = true;
    table_size_ = 0;
    table_state_ = TableState::empty;
    return true;
}

inline bool LookupTable1D::RefreshTableState()
{
    table_state_ = CheckTableState(x_axis_, y_table_);
    std::size_t x_size = ConvertSizeDataType(x_axis_.size());
    std::size_t y_size = ConvertSizeDataType(y_table_.size());
    if (table_state_ == TableState::valid)
    {
        table_valid_ = true;
        table_empty_ = false;
        table_size_ = x_size;
    }
    else
    {
        table_valid_ = false;
        table_empty_ = table_state_ == TableState::empty;
        table_size_ = (x_size < y_size) ? x_size : y_size;
    }
    return table_valid_;
}

LookupTable::TableState LookupTable1D::CheckTableState(const Eigen::RowVectorXd &input_vector1, const Eigen::RowVectorXd &input_vector2)
{
    if (input_vector1.size() == 0 || input_vector2.size() == 0)
    {
        return TableState::empty;
    }
    else if (input_vector1.size() > max_table_size_ || input_vector1.size() < 2)
    {
        return TableState::size_invalid; // x table size must be within the range of [2 max_axis_size]
    }
    else if (input_vector1.size() != input_vector2.size())
    {
        return TableState::size_not_match; // y table size must be equal to x table size
    }
    else if (!isStrictlyIncreasing(input_vector1))
    {
        return TableState::axis_not_increase; // x table data must be strictly increasing
    }
    else
    {
        return TableState::valid; // valid data for assignment
    }
}

// Configurations of lookup methods
void LookupTable1D::SetExtrapMethod(const ExtrapMethod &method)
{
    extrap_method_ = method;
    if (table_valid_)
    {
        lower_extrap_value_specify_ = y_table_(0);
        upper_extrap_value_specify_ = y_table_(y_table_.size() - 1);
    }
}
void LookupTable1D::SetExtrapMethod(const ExtrapMethod &method, const double &lower_value, const double &upper_value)
{
    extrap_method_ = method;
    lower_extrap_value_specify_ = lower_value;
    upper_extrap_value_specify_ = upper_value;
}

// Prelook
std::size_t LookupTable1D::PreLookup(const double &xvalue)
{
    size_t prelook_index = SearchIndex(xvalue, x_axis_, search_method_, prelook_index_);
    size_t x_size = ConvertSizeDataType(x_axis_.size());
    if (prelook_index >= 0 && prelook_index <= x_size) // valid prelookup
    {
        prelook_index_ = prelook_index; // store value
        if (search_method_ != SearchMethod::near)
            search_method_ = SearchMethod::near;    // set to near after binary search
    }
    else
    {
        prelook_index = prelook_index_;     // keep last value for invalid prelookup
        search_method_ = SearchMethod::bin; // reset to binary search after fail
    }
    return prelook_index;
}

// Interpolation between the two closest points
double LookupTable1D::Interpolation(const std::size_t &index, const double &xvalue)
{
    switch (interp_method_)
    {
    case InterpMethod::linear:
        return InterpolationLinear(index, xvalue);

    case InterpMethod::nearest:
        return InterpolationNearest(index, xvalue);

    case InterpMethod::next:
        return InterpolationNext(index, xvalue);

    case InterpMethod::previous:
        return InterpolationPrevious(index, xvalue);

    default:
        bool refresh = RefreshTableState();
        return lookup_result_;
    }
}
double LookupTable1D::InterpolationLinear(const std::size_t &index, const double &xvalue)
{
    // Interpolate 1D
    return Interpolate(xvalue, x_axis_(index - 1), x_axis_(index), y_table_(index - 1), y_table_(index));
}
double LookupTable1D::InterpolationNearest(const std::size_t &index, const double &xvalue)
{
    return ((xvalue - x_axis_[index - 1]) <= (x_axis_[index] - xvalue)) ? y_table_[index - 1] : y_table_[index];
}
double LookupTable1D::InterpolationNext(const std::size_t &index, const double &xvalue)
{
    return y_table_[index];
}
double LookupTable1D::InterpolationPrevious(const std::size_t &index, const double &xvalue)
{
    return y_table_[index - 1];
}

// Extrapolation if input is out of bounds
double LookupTable1D::Extrapolation(const std::size_t &index, const double &xvalue)
{
    switch (extrap_method_)
    {
    case ExtrapMethod::clip:
        return ExtrapolationClip(index);
    case ExtrapMethod::linear:
        return ExtrapolationLinear(index, xvalue);
    case ExtrapMethod::specify:
        return ExtrapolationSpecify(index, lower_extrap_value_specify_, upper_extrap_value_specify_);
    default:
        bool refresh = RefreshTableState();
        return lookup_result_;
    }
}
double LookupTable1D::ExtrapolationClip(const std::size_t &index)
{
    if (index == 0)
    {
        return y_table_(0);
    }
    else if (index == table_size_)
    {
        return y_table_(y_table_.size() - 1);
    }
    else
    {
        return lookup_result_; // if failure occurs, output the last value.
    }
}
double LookupTable1D::ExtrapolationLinear(const std::size_t &index, const double &xvalue)
{
    if (index == 0)
    {
        double x1 = x_axis_[0];
        double x2 = x_axis_[1];
        double y1 = y_table_[0];
        double y2 = y_table_[1];

        // Calculate the weight for linear extrapolation
        bool equal_zero = std::abs(x2 - x1) < epsilon_;
        double weight = equal_zero ? 0.5 : (xvalue - x1) / (x2 - x1);

        // Linearly extrapolate the y value based on the weight
        return y1 + weight * (y2 - y1);
    }
    else if (index == table_size_)
    {
        double x1 = x_axis_[table_size_ - 2];
        double x2 = x_axis_[table_size_ - 1];
        double y1 = y_table_[table_size_ - 2];
        double y2 = y_table_[table_size_ - 1];

        // Calculate the weight for linear extrapolation
        bool equal_zero = std::abs(x2 - x1) < epsilon_;
        double weight = equal_zero ? 0.5 : (xvalue - x1) / (x2 - x1);

        // Linearly extrapolate the y value based on the weight
        return y1 + weight * (y2 - y1);
    }
    else
    {
        return lookup_result_; // if failure occurs, output the last value.
    }
}
double LookupTable1D::ExtrapolationSpecify(const std::size_t &index, const double &lower_extrap_value, const double &upper_extrap_value)
{
    if (index == 0)
    {
        return lower_extrap_value;
    }
    else if (index == table_size_)
    {
        return upper_extrap_value;
    }
    else
    {
        return lookup_result_; // if failure occurs, output the last value.
    }
}

// Final function LookupTable
double LookupTable1D::Lookup(const double &xvalue)
{
    if (table_valid_)
    {
        size_t index = PreLookup(xvalue);
        if (index == 0 || index == table_size_) // in the case for extrapolation
        {
            lookup_result_ = Extrapolation(index, xvalue);
        }
        else
        {
            lookup_result_ = Interpolation(index, xvalue);
        }
        xvalue_ = xvalue; // restore the input value to member variable, for use in some cases.
    }
    else
    {
        bool refresh = RefreshTableState();
    }
    return lookup_result_;
}