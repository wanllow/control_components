#include "lookup_table.h"

std::size_t LookupTable::SearchIndex(const double &value, const Eigen::RowVectorXd &table, const SearchMethod &method, const std::size_t &last_index)
{
    switch (method)
    {
    case SearchMethod::seq:
    {
        std::size_t index = 0;
        for (index = 0; index != table.size(); ++index)
        {
            if (value <= table[index])
            {
                break;
            }
        }
        return index;
    }
    case SearchMethod::bin:
    {
        // Edge cases: value is out of bound

        if (value <= table(0))
        {
            return 0;
        }
        else if (value >= table(table.size() - 1))
        {
            return table.size();
        }

        std::size_t left = 0;
        std::size_t right = table.size() - 1;

        while (left < right)
        {
            std::size_t mid = left + (right - left) / 2; // Avoid overflow with safer midpoint calculation

            if (value <= table(mid))
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
    case SearchMethod::near:
    {
        std::size_t index = last_index; // Start searching from the last known index

        // Handle case where index is out of range (larger than table size)
        if (index >= table.size())
        {
            index = table.size(); // Set index to the last position
            while (index > 0 && value < table(index - 1))
            {
                --index; // Move backward until the value is within the table's range
            }
        }
        // Handle case where index is at the beginning of the table
        else if (index == 0)
        {
            while (index < table.size() && value > table(index))
            {
                ++index; // Move forward until the value fits within the table's range
            }
        }
        // Forward search: if the new value is greater than or equal to the current position
        else if (value >= table(index))
        {
            while (index < table.size() && value > table(index))
            {
                ++index; // Continue forward search until the value fits within the range
            }
        }
        // Backward search: if the new value is smaller than the previous position
        else if (value <= table(index - 1))
        {
            while (index > 0 && value < table(index - 1))
            {
                --index; // Continue backward search until the value fits within the range
            }
        }

        return index; // Return the index where the value fits within the table
    }
    default:
    {
        return last_index;
    }
    }
}

bool LookupTable::isStrictlyIncreasing(const Eigen::RowVectorXd &input_vector)
{
    for (size_t index = 1; index < input_vector.size(); ++index)
    {
        if (input_vector[index] - input_vector[index - 1] < epsilon_)
        {
            return false;
        }
    }
    return true;
}

bool LookupTable::ReportError()
{
    /* This function depends on the system, finish it as soon as possible */
    return table_valid_;
    /* End of this function */
}

double LookupTable::Interpolate(const double &xvalue, const double &x1, const double &x2, const double &y1, const double y2) const
{
    // Weight calculation
    bool equal_zero = std::abs(x2 - x1) < epsilon_;
    double weight = equal_zero ? 0.5 : (xvalue - x1) / (x2 - x1);

    // Linear interpolate
    return y1 + weight * (y2 - y1);
}
double LookupTable::Interpolate(const double &rvalue, const double &cvalue,
                                  const double &r1, const double &r2,
                                  const double &c1, const double &c2,
                                  const double &m11, const double &m12,
                                  const double &m21, const double &m22) const
{
    // Weight calculation
    bool equal_zero = std::abs(r2 - r1) < epsilon_;
    double rweight = equal_zero ? 0.5 : (rvalue - r1) / (r2 - r1);
    equal_zero = std::abs(c2 - c1) < epsilon_;
    double cweight = equal_zero ? 0.5 : (cvalue - c1) / (c2 - c1);
    // Linear interpolation
    return (1 - rweight) * ((1 - cweight) * m11 + cweight * m12) + rweight * ((1 - cweight) * m21 + cweight * m22);
}