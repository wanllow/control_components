#pragma once
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/Splines>
// #include "lookup_table_dependencies.h"
#include "lookup_table.h"

class LookupTable1D : public LookupTable
{
public:
    enum class TableState
    {
        empty = 0,          // empty table
        size_not_match = 1, // sizes of x_table and y_table not match
        size_invalid = 2,   // table size exceeds limit
        x_not_increase = 3, // x table is not strictly increasing
        valid = 4           // valid state
    };

    // Constructors and destructor
    LookupTable1D() = default;
    LookupTable1D(const Eigen::RowVectorXd &x_table, const Eigen::RowVectorXd &y_table) { SetTable(x_table, y_table); }
    LookupTable1D(const std::vector<double> &x_vec, const std::vector<double> &y_vec) { SetTable(x_vec, y_vec); }
    ~LookupTable1D() = default;

    // Get table state
    std::size_t size() const { return table_size_; }

    // Set and clear the table values

    void SetTable(const Eigen::RowVectorXd &x_table, const Eigen::RowVectorXd &y_table);
    void SetTable(const std::vector<double> &x_vec, const std::vector<double> &y_vec);
    void ClearTable() override;

    // Lookup table based on input, using current search, interp, and extrap methods
    double LookupTable(const double &x_value);

    // Configure the methods
    // void SetSearchMethod(const SearchMethod &method);
    // void SetInterpMethod(const InterpMethod &method);
    void SetExtrapMethod(const ExtrapMethod &method);
    void SetExtrapMethod(const ExtrapMethod &method, const double &lower_value, const double &upper_value);
    void SetLowerExtrapValue(const double &value) { lower_extrap_value_specify_ = value; }
    void SetUpperExtrapValue(const double &value) { upper_extrap_value_specify_ = value; }

private:
    Eigen::RowVectorXd x_table_;
    Eigen::RowVectorXd y_table_;
    double x_value_ = 0; // restore the input x value for lookup, use if necessary.
    double lookup_result_ = 0;
    std::size_t prelook_index_ = 0;

    // Methods for checking tables
    void RefreshTableState();
    bool isStrictlyIncreasing(const Eigen::RowVectorXd &input_vector);
    TableState CheckTableState(const Eigen::RowVectorXd &input_vector1, const Eigen::RowVectorXd &input_vector2);

    // Prelookup to find the index of the input value
    std::size_t PreLookup(const double &x_value);
    std::size_t SearchIndexSequential(const double &value, const Eigen::RowVectorXd &x_table);
    std::size_t SearchIndexBinary(const double &value, const Eigen::RowVectorXd &x_table);
    std::size_t SearchIndexNear(const double &value, const Eigen::RowVectorXd &x_table, const std::size_t &last_index);

    // Interpolation between the two closest points
    double Interpolation(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationLinear(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationNearest(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationNext(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationPrevious(const std::size_t &prelookup_index, const double &x_value);

    // Extrapolation if input is out of bounds
    double Extrapolation(const std::size_t &prelookup_index, const double &x_value);
    double ExtrapolationClip(const std::size_t &prelookup_index);
    double ExtrapolationLinear(const std::size_t &prelookup_index, const double &x_value);
    double ExtrapolationSpecify(const std::size_t &prelookup_index, const double &lower_extrap_value, const double &upper_extrap_value);

    // State of table
    bool table_empty_ = true;  // indicates the table is empty
    bool table_valid_ = false; // indicates the table is valid

    // Other parameters
    const std::size_t max_table_size_ = 1000000; // do not exceed 1M, though uint32_t can support up to 4294967295U.
    std::size_t table_size_ = 0U;                // length of table
    double lower_extrap_value_specify_ = 0;      // user specified value for out of boundary look up
    double upper_extrap_value_specify_ = 0;      // user specified value for out of boundary look up
};
