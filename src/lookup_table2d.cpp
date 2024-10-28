#include "lookup_table2d.h"

// Assign table data
LookupTable::AssignmentState LookupTable2D::AssignTableData(const Eigen::RowVectorXd &row_axis, const Eigen::RowVectorXd &col_axis, const Eigen::MatrixXd &map_matrix)
{
    if (CheckTableState(row_axis, col_axis, map_matrix) == TableState::valid)
    {
        row_axis_ = row_axis;
        col_axis_ = col_axis;
        map_matrix_ = map_matrix;
        bool refresh = RefreshTableState(); // redundant check, and refresh state in table
        return table_valid_ ? AssignmentState::success : AssignmentState::fail;
    }
    else
    {
        bool refresh = RefreshTableState();
        return table_valid_ ? AssignmentState::remain : AssignmentState::fail;
    }
}
LookupTable::AssignmentState LookupTable2D::AssignTableData(const std::vector<double> &row_vec, const std::vector<double> &col_vec, const std::vector<double> &map_vec)
{
    std::size_t rows = ConvertSizeDataType(row_vec.size());
    std::size_t cols = ConvertSizeDataType(col_vec.size());
    if (rows * cols == map_vec.size())
    {
        Eigen::RowVectorXd row_axis = Eigen::Map<const Eigen::RowVectorXd>(row_vec.data(), rows);
        Eigen::RowVectorXd col_axis = Eigen::Map<const Eigen::RowVectorXd>(col_vec.data(), cols);
        Eigen::MatrixXd map_matrix = Eigen::Map<const Eigen::MatrixXd>(map_vec.data(), rows, cols);
        return AssignTableData(row_axis, col_axis, map_matrix);
    }
    else
    {
        bool valid = RefreshTableState();                               // refresh the table
        return valid ? AssignmentState::remain : AssignmentState::fail; // return assignment state
    }
}

bool LookupTable2D::ClearTable()
{
    row_axis_.resize(0);
    col_axis_.resize(0);
    map_matrix_.resize(0, 0);
    table_empty_ = true;
    table_size_ = {0, 0};
    table_state_ = TableState::empty;
    return true;
}

inline bool LookupTable2D::RefreshTableState()
{
    table_state_ = CheckTableState(row_axis_, col_axis_, map_matrix_);
    std::size_t rows = ConvertSizeDataType(row_axis_.size());
    std::size_t cols = ConvertSizeDataType(col_axis_.size());
    table_size_ = {rows, cols};
    if (table_state_ == TableState::valid)
    {
        table_valid_ = true;
        table_empty_ = false;
    }
    else
    {
        table_valid_ = false;
        table_empty_ = table_state_ == TableState::empty;
    }
    return table_valid_;
}

LookupTable::TableState LookupTable2D::CheckTableState(const Eigen::RowVectorXd &input_vector1, const Eigen::RowVectorXd &input_vector2, const Eigen::MatrixXd &input_matrix)
{
    if (input_vector1.data() == nullptr && input_vector2.data() == nullptr && input_matrix.data() == nullptr)
    {
        return LookupTable::TableState::empty;
    }
    else if (input_vector1.size() < 2 || input_vector1.size() > max_table_size_ || input_vector2.size() < 2 || input_vector2.size() > max_table_size_ || input_matrix.rows() < 2 || input_matrix.rows() > max_table_size_ || input_matrix.cols() < 2 || input_matrix.cols() > max_table_size_)
    {
        return LookupTable::TableState::size_invalid;
    }
    else if (input_vector1.size() != input_matrix.rows() || input_vector2.size() != input_matrix.cols())
    {
        return LookupTable::TableState::size_not_match;
    }
    else if (!isStrictlyIncreasing(input_vector1) || !isStrictlyIncreasing(input_vector2))
    {
        return LookupTable::TableState::axis_not_increase;
    }
    else
    {
        return LookupTable::TableState::valid;
    }
}

LookupTable::MatrixIndex LookupTable2D::PreLookup(const double &row_value, const double &col_value)
{
    std::size_t row = SearchIndex(row_value, row_axis_, search_method_, prelook_index_.rows());
    std::size_t col = SearchIndex(col_value, col_axis_, search_method_, prelook_index_.cols());
    size_t max_row = ConvertSizeDataType(row_axis_.size());
    size_t max_col = ConvertSizeDataType(col_axis_.size());
    if (row >= 0 && row <= max_row && col >= 0 && col <= max_col)
    {
        if (search_method_ != SearchMethod::near)
        {
            search_method_ = SearchMethod::near; // set to near after binary search
        }
        prelook_index_ = {row, col}; // restore prelookup value
    }
    else
    {
        search_method_ = SearchMethod::bin;
    }
    return prelook_index_;
}

// Interpolation between the two closest points
double LookupTable2D::Interpolation(const MatrixIndex &prelookup_index, const double &rvalue, const double &cvalue)
{
    // Setting calculation range
    const std::size_t &rindex = prelookup_index.rows();
    const std::size_t &cindex = prelookup_index.cols();
    const double &r1 = row_axis_(rindex - 1);
    const double &r2 = row_axis_(rindex);
    const double &c1 = col_axis_(cindex - 1);
    const double &c2 = col_axis_(cindex);
    const double &m11 = map_matrix_(rindex - 1, cindex - 1);
    const double &m12 = map_matrix_(rindex - 1, cindex);
    const double &m21 = map_matrix_(rindex, cindex - 1);
    const double &m22 = map_matrix_(rindex, cindex);
    // Weight calculation
    bool equal_zero = std::abs(r2 - r1) < epsilon_;
    double rweight = equal_zero ? 0.5 : (rvalue - r1) / (r2 - r1);
    equal_zero = std::abs(c2 - c1) < epsilon_;
    double cweight = equal_zero ? 0.5 : (rvalue - c1) / (c2 - c1);
    // Interpolate
    double result = (1 - rweight) * ((1 - cweight) * m11 + cweight * m12) +
                    rweight * ((1 - cweight) * m21 + cweight * m22);
    return result;
}

double LookupTable2D::Extrapolation(const MatrixIndex &prelookup_index, const double &row_value, const double &col_value)
{
    // Setting calculation range
    const std::size_t &rindex = prelookup_index.rows();
    const std::size_t &cindex = prelookup_index.cols();
    const std::size_t rsize = table_size_.rows();
    const std::size_t csize = table_size_.cols();
    double result = 0;

    if (rindex < 1)
    {
        if (cindex < 1)
        {
            result = map_matrix_(0, 0);
        }
        else if (cindex >= csize)
        {
            result = map_matrix_(0, csize - 1);
        }
        else
        {
            const double &c1 = col_axis_(cindex - 1);
            const double &c2 = col_axis_(cindex);
            const double &m1 = map_matrix_(0, cindex - 1);
            const double &m2 = map_matrix_(0, cindex);

            bool equal_zero = std::abs(c2 - c1) < epsilon_;
            double weight = equal_zero ? 0.5 : (col_value - c1) / (c2 - c1);
            result = m1 + weight * (m2 - m1);
        }
    }
    else if(rindex > rsize)
    {
        if (cindex < 1)
        {
            result = map_matrix_(rsize - 1,0);
        }
        if (cindex >= csize)
        {
            result = map_matrix_(rsize - 1, csize - 1);
        }
        else
        {
            const double &c1 = col_axis_(cindex - 1);
            const double &c2 = col_axis_(cindex);
            const double &m1 = map_matrix_(rsize - 1, cindex - 1);
            const double &m2 = map_matrix_(rsize - 1, cindex);

            bool equal_zero = std::abs(c2 - c1) < epsilon_;
            double weight = equal_zero ? 0.5 : (col_value - c1) / (c2 - c1);
            result = m1 + weight * (m2 - m1);
        }
    }
    else
    {
        if (cindex < 1)
        {
            const double &r1 = row_axis_(rindex - 1);
            const double &r2 = row_axis_(rindex);
            const double &m1 = map_matrix_(rindex - 1, 0);
            const double &m2 = map_matrix_(rindex, 0);

            bool equal_zero = std::abs(r2 - r1) < epsilon_;
            double weight = equal_zero ? 0.5 : (col_value - r1) / (r2 - r1);
            result = m1 + weight * (m2 - m1);
        }
        else if(cindex >= csize)
        {
            const double &r1 = row_axis_(rindex - 1);
            const double &r2 = row_axis_(rindex);
            const double &m1 = map_matrix_(rindex - 1, csize - 1);
            const double &m2 = map_matrix_(rindex, csize - 1);

            bool equal_zero = std::abs(r2 - r1) < epsilon_;
            double weight = equal_zero ? 0.5 : (col_value - r1) / (r2 - r1);
            result = m1 + weight * (m2 - m1);
        }
    }
    return result;
}