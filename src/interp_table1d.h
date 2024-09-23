#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <stdexcept>

class InterpTable1D
{

    /** Forward declarations of enums **/
private:
    enum class SearchMethod;
    enum class InterpMethod;
    enum class ExtrapMethod;
    enum class TableState;

public:
    // Constructors and destructor
    InterpTable1D();
    InterpTable1D(const std::vector<double> &x_table, const std::vector<double> &y_table);
    ~InterpTable1D() = default;

    // Get table state
    std::size_t size();
    bool valid();
    bool empty();
    bool IfWriteSuccess();

    // Set and clear the table values
    void SetTableValue(const std::vector<double> &x_table, const std::vector<double> &y_table);
    void ClearValue();

    // Lookup table based on input, using current search, interp, and extrap methods
    double LookupTable(const double& x_value);

    // Configure the methods
    void SetSearchMethod(const SearchMethod &method);
    void SetInterpMethod(const InterpMethod &method);
    void SetExtrapMethod(const ExtrapMethod &method);
    void SetExtrapMethod(const ExtrapMethod &method, const double &lower_value, const double &upper_value);
    void SetLowerExtrapValue(const double &value);
    void SetUpperExtrapValue(const double &value);


private:
    std::vector<double> x_table_;
    std::vector<double> y_table_;
    double x_value_ = 0;                // restore the input x value for lookup, use if necessary.
    double lookup_result_ = 0;
    std::size_t prelook_index_ = 0;

    // Methods for checking tables
    void RefreshTableState();
    bool isStrictlyIncreasing(const std::vector<double> &input_vector);
    // bool CheckTableTypes(const vector<double>& input_vector1, const vector<double>& input_vector2);
    TableState CheckTableState(const std::vector<double> &input_vector1, const std::vector<double> &input_vector2);

    // Prelookup to find the index of the input value
    std::size_t PreLookup(const double &x_value);
    std::size_t SearchIndexSequential(const double &value, const std::vector<double> &x_table);
    std::size_t SearchIndexBinary(const double &value, const std::vector<double> &x_table);
    std::size_t SearchIndexNear(const double &value, const std::vector<double> &x_table, const std::size_t &last_index);

    // Interpolation between the two closest points
    double Interpolation(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationLinear(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationNearest(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationNext(const std::size_t &prelookup_index, const double &x_value);
    double InterpolationPrevious(const std::size_t &prelookup_index, const double &x_value);

    // Extrapolation if input is out of bounds
    double Extrapolation(const std::size_t& prelookup_index, const double& x_value);
    double ExtrapolationClip(const std::size_t& prelookup_index);
    double ExtrapolationLinear(const std::size_t& prelookup_index, const double& x_value);
    double ExtrapolationSpecify(const std::size_t& prelookup_index, const double& lower_extrap_value, const double& upper_extrap_value);

    // Currently selected methods
    SearchMethod search_method_ = SearchMethod::bin;
    InterpMethod interp_method_ = InterpMethod::linear;
    ExtrapMethod extrap_method_ = ExtrapMethod::clip;

    // State of table
    bool table_empty_ = true;        // indicates the table is empty
    bool table_valid_ = false;       // indicates the table is valid
    bool set_value_success_ = false; // indicates whether a successful SetTableValue has been executed.

    // Other parameters
    const std::size_t max_table_size_ = 1000000;    // do not exceed 1M, though uint32_t can support up to 4294967295U.
    std::size_t table_size_ = 0U;                   // length of table
    double lower_extrap_value_specify_ = 0;         // user specified value for out of boundary look up
    double upper_extrap_value_specify_ = 0;         // user specified value for out of boundary look up

    enum class SearchMethod
    {
        seq = 0, // sequential search, from begin to end
        bin = 1, // binary search
        near = 2 // sequential search based on last value
    };

    enum class InterpMethod
    {
        nearest = 0,    // use nearest point value
        linear = 1,     // linear interpolation
        next = 2,       // use the next point value
        previous = 3    // use the previous point value
    };

    enum class ExtrapMethod
    {
        clip = 0,       // use end value
        linear = 1,     // linear extrpolation
        specify = 2     // user specify the values
    };

    enum class TableState
    {
        empty = 0,              // empty table
        size_not_match = 1,     // sizes of x_table and y_table not match
        size_invalid = 2,  // table size exceeds limit
        x_not_increase = 3,// x table is not strictly increasing
        valid = 4               // valid state
    };
};
