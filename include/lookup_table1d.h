#pragma once
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/Splines>
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

    // Constructors and destructor, set_state is left for debugging
    LookupTable1D() = default;
    LookupTable1D(const Eigen::RowVectorXd &x_table, const Eigen::RowVectorXd &y_table) { SetState set_state = SetTable(x_table, y_table); }
    LookupTable1D(const std::vector<double> &x_vec, const std::vector<double> &y_vec) { SetState set_state = SetTable(x_vec, y_vec); }
    ~LookupTable1D() = default;

    // Get table state
    std::size_t size() const { return table_size_; }
    TableState state() const { return table_state_; }

    // Set and clear the table values
    SetState SetTable(const Eigen::RowVectorXd &x_table, const Eigen::RowVectorXd &y_table);
    SetState SetTable(const std::vector<double> &x_vec, const std::vector<double> &y_vec);
    bool ClearTable() override;

    // Lookup table based on input, using current search, interp, and extrap methods
    double LookupTable(const double &x_value);

    // Configure the methods
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
    TableState CheckTableState(const Eigen::RowVectorXd &input_vector1, const Eigen::RowVectorXd &input_vector2);

    // Prelookup to find the index of the input value
    std::size_t PreLookup(const double &x_value);

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

    // Other parameters
    std::size_t table_size_ = 0U; // length of table
    TableState table_state_ = TableState::empty;
    double lower_extrap_value_specify_ = 0; // user specified value for out of boundary look up
    double upper_extrap_value_specify_ = 0; // user specified value for out of boundary look up
};
