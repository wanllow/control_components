#include "interp_table1d.h"

using std::vector;
using std::size_t;
// Constructors
InterpTable1D::InterpTable1D() {}
InterpTable1D::InterpTable1D(const vector<double> &x_table, const vector<double> &y_table)
{
    SetTableValue(x_table, y_table);
}

// Get table members
size_t InterpTable1D::size()
{
    return table_size_;
}
bool InterpTable1D::valid()
{
    return table_valid_;
}
bool InterpTable1D::empty()
{
    return table_empty_;
}

// Set table values to new input values, validate first then set value in.
void InterpTable1D::SetTableValue(const vector<double> &x_table, const vector<double> &y_table)
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

void InterpTable1D::RefreshTableState()
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

InterpTable1D::TableState InterpTable1D::CheckTableState(const vector<double> &input_vector1, const vector<double> &input_vector2)
{
    if (input_vector1.empty() || input_vector2.empty())
    {
        return TableState::empty;
    }
    else if (input_vector1.size() > max_table_size_ || input_vector2.size() > max_table_size_)
    {
        return TableState::size_exceed_limit;
    }
    else if (input_vector1.size() != input_vector2.size())
    {
        return TableState::size_not_equal;
    }
    else if (!isStrictlyIncreasing(input_vector1))
    {
        return TableState::x_not_mono_increase;
    }
    else
    {
        return TableState::valid;
    }
}

bool InterpTable1D::isStrictlyIncreasing(const vector<double> &input_vector)
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

// configuration of lookup methods
void InterpTable1D::SetSearchMethod(const SearchMethod &method)
{
    search_method_ = method;
}
void InterpTable1D::SetInterpMethod(const InterpMethod &method)
{
    interp_method_ = method;
}
void InterpTable1D::SetExtrapMethod(const ExtrapMethod &method)
{
    extrap_method_ = method;
    if (table_valid_)
    {
        lower_extrap_value_specify_ = y_table_.front();
        upper_extrap_value_specify_ = y_table_.back();
    }
}
void InterpTable1D::SetExtrapMethod(const ExtrapMethod& method, const double& lower_value, const double& upper_value)
{
    extrap_method_ = method;
    lower_extrap_value_specify_ = lower_value;
    upper_extrap_value_specify_ = upper_value;
}
void InterpTable1D::SetLowerExtrapValue(const double &value)
{
    lower_extrap_value_specify_ = value;
}
void InterpTable1D::SetUpperExtrapValue(const double &value)
{
    upper_extrap_value_specify_ = value;
}
