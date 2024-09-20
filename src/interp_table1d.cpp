#include "interp_table1d.h"

InterpTable1D::InterpTable1D(const vector<double> &x_table, const vector<double> &y_table)
    : x_table_(x_table), y_table_(y_table)
{
    CheckTableSize();
}

void InterpTable1D::SetValue(const vector<double> &x_table, const vector<double> &y_table)
{
    CheckTableSize();
    SetTableX(x_table);
    SetTableY(y_table);
}

void InterpTable1D::SetTableX(const vector<double> &x_table)
{
    x_table_ = x_table;
}

void InterpTable1D::SetTableY(const vector<double> &y_table)
{
    y_table_ = y_table;
}

std::size_t InterpTable1D::GetTableSize(const vector<double>& input_vector1, const vector<double>& input_vector2)
{
    if (input_vector1.size() == input_vector2.size())
    {
        return input_vector1.size();
    }
    else
    {
        return 0;
    }
}

std::size_t InterpTable1D::GetTableSize()
{
    std::size_t temp_size =  GetTableSize(x_table_, y_table_);
}

void InterpTable1D::CheckTableSize()
{
    std::size_t temp_size = GetTableSize();
    if ((temp_size > 0) && temp_size < max_table_size_)
    {
        table_state_ = false;
    }
    else
    {
        table_state_ = false;
    }
}