#pragma once
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <Eigen/Dense>
#include <unsupported/Eigen/Splines>

class LookupTable
{
public:
    // Important states
    enum class SearchMethod
    {
        seq = 0, // sequential search, from begin to end
        bin = 1, // binary search
        near = 2 // sequential search based on last value
    };

    enum class InterpMethod
    {
        nearest = 0, // use nearest point value
        linear = 1,  // linear interpolation
        next = 2,    // use the next point value
        previous = 3 // use the previous point value
    };

    enum class ExtrapMethod
    {
        clip = 0,   // use end value
        linear = 1, // linear extrpolation
        specify = 2 // user specify the values
    };

    enum class AssignmentState
    {
        fail = -1,  // new value not set, old value invalid
        remain = 0, // new value not set, old value valid
        success = 1 // new value set successfully
    };
    enum class TableState
    {
        empty = 0,              // empty table
        size_not_match = -1,    // sizes of x_table and y_table not match
        size_invalid = -2,      // table size exceeds the range [2 limit]
        axis_not_increase = -3, // x table is not strictly increasing
        valid = 1               // valid state
    };
    class MatrixIndex
    {
    public:
        // Constructors and destructors
        MatrixIndex(const std::size_t &rows, const std::size_t &cols) : rows_{rows}, cols_{cols} {}
        // explicit MatrixIndex(Eigen::Index rows, Eigen::Index cols) : rows_{static_cast<std::size_t>(rows)}, cols_{static_cast<std::size_t>(cols)} {}
        ~MatrixIndex() = default;
        // Access to members
        std::size_t rows() const { return rows_; }
        std::size_t cols() const { return cols_; }
        // Assignment operator
        MatrixIndex &operator=(const MatrixIndex &other)
        {
            if (this != &other) // avoid self assignment
            {
                rows_ = other.rows_;
                cols_ = other.cols_;
            }
            return *this;
        }
        // Comparator operator
        bool operator==(const MatrixIndex &other) const
        {
            return rows_ == other.rows_ && cols_ == other.cols_;
        }
        bool operator!=(const MatrixIndex &other) const
        {
            return rows_ != other.rows_ || cols_ != other.cols_;
        }

    private:
        std::size_t rows_ = 0;
        std::size_t cols_ = 0;
    };

    // Constructors and destructors
    LookupTable() = default;
    virtual ~LookupTable() = default;

    // Get present state
    bool valid() const { return table_valid_; }
    bool empty() const { return table_empty_; }
    TableState state() const { return table_state_; }

    // Set methods for search, interpolation, and extrapolation
    void SetSearchMethod(const SearchMethod &method) { search_method_ = method; }
    void SetInterpMethod(const InterpMethod &method) { interp_method_ = method; }
    virtual void SetExtrapMethod(const ExtrapMethod &method) { extrap_method_ = method; }
    void SetEpsilon(const double &epsilon) { epsilon_ = epsilon > 0 ? epsilon : epsilon_; }

    // virtual void SetTable() = 0;   // SetTable without any input parameter, it's complete virtual function
    virtual bool ClearTable() = 0; // ClearTable may be different for 1dTable and 2dTable

    // Three main search index functions
    std::size_t SearchIndex(const double &value, const Eigen::RowVectorXd &table, const SearchMethod &method, const std::size_t &last_index = 0);

    // Report error in case of fault
    bool ReportError();

protected:
    // Common member variables for table configuration
    bool table_empty_ = true;
    bool table_valid_ = false;
    TableState table_state_ = TableState::empty;
    AssignmentState table_assigned_ = AssignmentState::remain;

    // Methods
    SearchMethod search_method_ = SearchMethod::bin;
    InterpMethod interp_method_ = InterpMethod::linear;
    ExtrapMethod extrap_method_ = ExtrapMethod::clip;

    // Important threthold
    double epsilon_ = std::numeric_limits<double>::epsilon();
    const std::size_t max_table_size_ = 1000000U; // do not exceed 1M, uint32_t can support up to 4294967295U.

    // Functions commonly used
    bool isStrictlyIncreasing(const Eigen::RowVectorXd &input_vector);

    // Convert Eigen::Index (long int) type to std::size_t (unsigned long int), avoid negative integers
    inline std::size_t ConvertSizeDataType(const Eigen::Index &eigen_index) { return static_cast<std::size_t>(std::max(eigen_index, Eigen::Index(0))); }
};