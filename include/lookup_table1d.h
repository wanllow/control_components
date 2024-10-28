#pragma once
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/Splines>
#include "lookup_table.h"

class LookupTable1D : public LookupTable
{
public:
    // Constructors and destructor, set_state is left for debugging
    LookupTable1D() = default;
    LookupTable1D(const std::size_t &size) : x_axis_{Eigen::RowVectorXd::LinSpaced(size,1,size)}, y_table_{Eigen::RowVectorXd::Zero(size)} {}
    LookupTable1D(const Eigen::RowVectorXd &x_axis, const Eigen::RowVectorXd &y_table) { AssignmentState assigned = AssignTableData(x_axis, y_table); }
    LookupTable1D(const std::vector<double> &x_vec, const std::vector<double> &y_vec) { AssignmentState assigned = AssignTableData(x_vec, y_vec); }
    ~LookupTable1D() = default;

    // Get table state
    std::size_t size() const { return table_size_; }

    // Set and clear the table values
    AssignmentState AssignTableData(const Eigen::RowVectorXd &x_axis, const Eigen::RowVectorXd &y_table);
    AssignmentState AssignTableData(const std::vector<double> &x_vec, const std::vector<double> &y_vec);
    bool ClearTable() override;

    // Lookup table based on input, using current search, interp, and extrap methods
    double LookupTable(const double &x_value);

    // Configure the methods
    void SetExtrapMethod(const ExtrapMethod &method);
    void SetExtrapMethod(const ExtrapMethod &method, const double &lower_value, const double &upper_value);
    void SetLowerExtrapValue(const double &value) { lower_extrap_value_specify_ = value; }
    void SetUpperExtrapValue(const double &value) { upper_extrap_value_specify_ = value; }

private:
    // Core members
    Eigen::RowVectorXd x_axis_;
    Eigen::RowVectorXd y_table_;
    double x_value_ = 0;            // restore the input x value for lookup, use if necessary.
    double lookup_result_ = 0;      // restore output value.
    std::size_t prelook_index_ = 0; // restore prelook index value.
    // Other parameters
    std::size_t table_size_ = 0U; // length of table
    double lower_extrap_value_specify_ = 0; // user specified value for out of boundary look up
    double upper_extrap_value_specify_ = 0; // user specified value for out of boundary look up

    // Methods for checking tables
    bool RefreshTableState();
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
};
