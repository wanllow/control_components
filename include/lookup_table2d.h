#pragma once
#include <vector>
#include <Eigen/Dense>
#include "lookup_table.h"

class LookupTable2D : public LookupTable
{
public:
    // Constructors and destructors
    LookupTable2D() = default;
    LookupTable2D(const std::size_t &rows, const std::size_t &cols) : row_axis_{Eigen::RowVectorXd::LinSpaced(rows, 1, rows)}, col_axis_{Eigen::RowVectorXd::LinSpaced(cols, 1, cols)}, map_matrix_{Eigen::MatrixXd::Zero(rows, cols)} {}
    LookupTable2D(const Eigen::RowVectorXd row_axis, const Eigen::RowVectorXd col_axis, const Eigen::MatrixXd &map_matrix) { table_assigned_ = AssignTableData(row_axis, col_axis, map_matrix); }
    LookupTable2D(const std::vector<double> &row_axis, const std::vector<double> &col_axis, const std::vector<double> &map_matrix) { table_assigned_ = AssignTableData(row_axis, col_axis, map_matrix); }
    ~LookupTable2D() = default;

    // Get table state
    MatrixIndex size() const { return table_size_; }
    std::size_t rows() const { return table_size_.rows(); }
    std::size_t cols() const { return table_size_.cols(); }

    // Set and clear the table values
    AssignmentState AssignTableData(const Eigen::RowVectorXd &row_axis, const Eigen::RowVectorXd &col_axis, const Eigen::MatrixXd &mat_matrix);
    AssignmentState AssignTableData(const std::vector<double> &row_vec, const std::vector<double> &col_vec, const std::vector<double> &map_vec);
    bool ClearTable() override;

    // Lookup table based on input, using current search, interp, and extrap methods
    double Lookup(const double &rvalue, const double &cvalue);

private:
    // Core members
    Eigen::RowVectorXd row_axis_; // the row axis
    Eigen::RowVectorXd col_axis_; // the column axis
    Eigen::MatrixXd map_matrix_;  // the map matrix
    double row_value_ = 0;        // restore the input x value for lookup
    double col_value_ = 0;        // restore the input y value for lookup
    double lookup_result_ = 0;    // restore lookup result
    MatrixIndex prelook_index_{0, 0};
    // Table state members
    MatrixIndex table_size_{0, 0};

    // Methods for checking tables
    bool RefreshTableState();
    TableState CheckTableState(const Eigen::RowVectorXd &input_vector1, const Eigen::RowVectorXd &input_vector2, const Eigen::MatrixXd &input_matrix);

    // Prelookup to find the index of the input value
    MatrixIndex PreLookup(const double &row_value, const double &col_value);

    // Interpolation between the two closest points
    double Interpolation(const MatrixIndex &prelookup_index, const double &row_value, const double &col_value);

    // Extrapolation if input is out of bounds, only support clip method
    double Extrapolation(const MatrixIndex &prelookup_index, const double &row_value, const double &col_value);
};