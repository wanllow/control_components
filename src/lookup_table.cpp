#include "lookup_table.h"

std::size_t LookupTable::SearchIndex(const double &value, const Eigen::RowVectorXd &table, const SearchMethod &method, const std::size_t &last_index)
{
    std::size_t index = last_index;
    switch (method)
    {
    case SearchMethod::seq:
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

        size_t left = 0;
        size_t right = table.size() - 1;

        while (left < right)
        {
            size_t mid = left + (right - left) / 2; // Avoid overflow with safer midpoint calculation

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
        return -1;
    }
    }
}