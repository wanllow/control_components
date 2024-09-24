#include "lookup_table1d.h"
#include <algorithm>

using std::size_t;
using std::vector;
// Constructors
LookupTable1D::LookupTable1D() {}
LookupTable1D::LookupTable1D(const vector<double> &x_table, const vector<double> &y_table)
{
    SetTableValue(x_table, y_table);
}

// Get table members
size_t LookupTable1D::size()
{
    return table_size_;
}
bool LookupTable1D::valid()
{
    return table_valid_;
}
bool LookupTable1D::empty()
{
    return table_empty_;
}
bool LookupTable1D::IfWriteSuccess()
{
    return set_value_success_;
}

// Set table values to new input values, validate first then set value in.
void LookupTable1D::SetTableValue(const vector<double> &x_table, const vector<double> &y_table)
{
    if (CheckTableState(x_table, y_table) == TableState::valid)
    {
        x_table_ = x_table;
        y_table_ = y_table;
        set_value_success_ = true;
        RefreshTableState();
    }
    else
    {
        // if the input value is invalid, check the current table value to set the table_state flag
        // and reset the set_value_success_ flag to indicate a failed SetTableValue has been executed.
        table_valid_ = (CheckTableState(x_table_, y_table_) == TableState::valid);
        set_value_success_ = false;
    }
}

void LookupTable1D::RefreshTableState()
{
    table_valid_ = (CheckTableState(x_table_, y_table_) == TableState::valid);
    if (table_valid_)
    {
        table_valid_ = true;
        table_empty_ = false;
        table_size_ = x_table_.size();
    }
    else
    {
        table_valid_ = false;
        table_empty_ = true;
        table_size_ = 0;
    }
}

LookupTable1D::TableState LookupTable1D::CheckTableState(const vector<double> &input_vector1, const vector<double> &input_vector2)
{
    if (input_vector1.empty() || input_vector2.empty())
    {
        return TableState::empty;
    }
    else if (input_vector1.size() > max_table_size_ || input_vector1.size() < 2)
    {
        return TableState::size_invalid;    // x table size must be within the range of [2 max_table_size]
    }
    else if (input_vector1.size() != input_vector2.size())
    {
        return TableState::size_not_match;  // y table size must be equal to x table size
    }
    else if (!isStrictlyIncreasing(input_vector1))
    {
        return TableState::x_not_increase;  // x table data must be strictly increasing
    }
    else
    {
        return TableState::valid;           // valid data for assignment
    }
}

bool LookupTable1D::isStrictlyIncreasing(const vector<double> &input_vector)
{
    for (size_t index = 1; index < input_vector.size(); ++index)
    {
        if (input_vector[index - 1] >= input_vector[index])
        {
            return false;
        }
    }
    return true;
}

// Configurations of lookup methods
void LookupTable1D::SetSearchMethod(const SearchMethod &method)
{
    search_method_ = method;
}
void LookupTable1D::SetInterpMethod(const InterpMethod &method)
{
    interp_method_ = method;
}
void LookupTable1D::SetExtrapMethod(const ExtrapMethod &method)
{
    extrap_method_ = method;
    if (table_valid_)
    {
        lower_extrap_value_specify_ = y_table_.front();
        upper_extrap_value_specify_ = y_table_.back();
    }
}
void LookupTable1D::SetExtrapMethod(const ExtrapMethod &method, const double &lower_value, const double &upper_value)
{
    extrap_method_ = method;
    lower_extrap_value_specify_ = lower_value;
    upper_extrap_value_specify_ = upper_value;
}
void LookupTable1D::SetLowerExtrapValue(const double &value)
{
    lower_extrap_value_specify_ = value;
}
void LookupTable1D::SetUpperExtrapValue(const double &value)
{
    upper_extrap_value_specify_ = value;
}

std::size_t LookupTable1D::PreLookup(const double &x_value)
{
    size_t prelook_index = 0;
    switch (search_method_)
    {
    case SearchMethod::seq:
    {
        prelook_index = SearchIndexSequential(x_value, x_table_); // Sequential search
        break;
    }

    case SearchMethod::bin:
    {
        prelook_index = SearchIndexBinary(x_value, x_table_); // Binary search
        break;
    }

    case SearchMethod::near:
    {
        prelook_index = SearchIndexNear(x_value, x_table_, lookup_result_); // Search based on last index
        break;
    }

    default:
        prelook_index = prelook_index_; // return the last value if failure occured.
        RefreshTableState();
    }

    return prelook_index;
}

// Local function: search index using sequential method
size_t LookupTable1D::SearchIndexSequential(const double &value, const vector<double> &table)
{
    size_t index = 0;
    for (index = 0; index != table.size(); ++index)
    {
        if (value <= table[index])
        {
            break;
        }
    }
    return index;
}

// Local function: search index using binary method
size_t LookupTable1D::SearchIndexBinary(const double &value, const vector<double> &table)
{
    // Edge cases: value is out of bound

    if (value <= table.front())
    {
        return 0;
    }
    else if (value >= table.back())
    {
        return table.size();
    }

    size_t left = 0;
    size_t right = table.size() - 1;

    while (left < right)
    {
        size_t mid = left + (right - left) / 2; // Avoid overflow with safer midpoint calculation

        if (value == table[mid])
        {
            return mid; // Exact match found
        }
        else if (value < table[mid])
        {
            right = mid; // Narrow down to the left half
        }
        else
        {
            left = mid + 1; // Narrow down to the right half
        }
    }
    return left;
}

// Local function: search index using last search result
size_t LookupTable1D::SearchIndexNear(const double &value, const vector<double> &table, const size_t &last_index)
{
    size_t index = last_index;

    if (value <= table.front())
    {
        return 0;
    }
    else if (value >= table.back())
    {
        return table.size();
    }

    // Search backward if the previous value is table.size()
    if (last_index == table.size())
    {
        while (value < table[index - 1])
        {
            --index;
        }
        return index;
    }

    // Search forward if the new value is larger than the previous value
    if (value >= table[index])
    {
        while (index < table.size() && value > table[index])
        {
            ++index;
        }
    }
    // Search backward if the new value is smaller than the previous value
    else if (value <= table[index - 1])
    {
        while (index > 0 && value < table[index - 1])
        {
            --index;
        }
    }
    return index;
}

// Interpolation between the two closest points
double LookupTable1D::Interpolation(const std::size_t &index, const double &x_value)
{
    switch (interp_method_)
    {
    case InterpMethod::linear:
        return InterpolationLinear(index, x_value);

    case InterpMethod::nearest:
        return InterpolationNearest(index, x_value);

    case InterpMethod::next:
        return InterpolationNext(index, x_value);

    case InterpMethod::previous:
        return InterpolationPrevious(index, x_value);

    default:
        RefreshTableState();
        return lookup_result_;
    }
}
double LookupTable1D::InterpolationLinear(const std::size_t &index, const double &x_value)
{
    // Retrieve the two nearest points for interpolation
    double x1 = x_table_[index - 1];
    double x2 = x_table_[index];
    double y1 = y_table_[index - 1];
    double y2 = y_table_[index];

    // Calculate the weight for linear interpolation
    double delta_x = x2 - x1;
    double weight = (delta_x == 0) ? 0.5 : (x_value - x1) / delta_x;

    // Linearly interpolate the y value based on the weight
    return y1 + weight * (y2 - y1);
}
double LookupTable1D::InterpolationNearest(const std::size_t &index, const double &x_value)
{
    return ((x_value - x_table_[index - 1]) <= (x_table_[index] - x_value)) ? y_table_[index - 1] : y_table_[index];
}
double LookupTable1D::InterpolationNext(const std::size_t &index, const double &x_value)
{
    return y_table_[index];
}
double LookupTable1D::InterpolationPrevious(const std::size_t &index, const double &x_value)
{
    return y_table_[index - 1];
}

// Extrapolation if input is out of bounds
double LookupTable1D::Extrapolation(const std::size_t &index, const double &x_value)
{
    switch (extrap_method_)
    {
    case ExtrapMethod::clip:
        return ExtrapolationClip(index);
    case ExtrapMethod::linear:
        return ExtrapolationLinear(index, x_value);
    case ExtrapMethod::specify:
        return ExtrapolationSpecify(index, lower_extrap_value_specify_, upper_extrap_value_specify_);
    default:
        RefreshTableState();
        return lookup_result_;
    }
}
double LookupTable1D::ExtrapolationClip(const std::size_t &index)
{
    if (index == 0)
    {
        return y_table_.front();
    }
    else if (index == table_size_)
    {
        return y_table_.back();
    }
    else
    {
        return lookup_result_; // if failure occurs, output the last value.
    }
}
double LookupTable1D::ExtrapolationLinear(const std::size_t &index, const double &x_value)
{
    if (index == 0)
    {
        double x1 = x_table_[0];
        double x2 = x_table_[1];
        double y1 = y_table_[0];
        double y2 = y_table_[1];

        // Calculate the weight for linear extrapolation
        double delta_x = x2 - x1;
        double weight = (delta_x == 0) ? 0.5 : (x_value - x1) / delta_x;

        // Linearly extrapolate the y value based on the weight
        return y1 + weight * (y2 - y1);
    }
    else if (index == table_size_)
    {
        double x1 = x_table_[table_size_ - 2];
        double x2 = x_table_[table_size_ - 1];
        double y1 = y_table_[table_size_ - 2];
        double y2 = y_table_[table_size_ - 1];

        // Calculate the weight for linear extrapolation
        double delta_x = x2 - x1;
        double weight = (delta_x == 0) ? 0.5 : (x_value - x1) / delta_x;

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
double LookupTable1D::LookupTable(const double &x_value)
{
    if (table_valid_)
    {
        size_t index = PreLookup(x_value);
        if (index == 0 || index == table_size_) // in the case for extrapolation
        {
            lookup_result_ = Extrapolation(index, x_value);
        }
        else
        {
            lookup_result_ = Interpolation(index, x_value);
        }
        x_value_ = x_value; // restore the input value to member variable, for use in some cases.
    }
    else
    {
        RefreshTableState();
    }
    return lookup_result_;
}