#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

using std::vector;
enum class SearchMethod
{
    seq = 0,    // sequential search, from begin to end
    bin = 1,    // binary search
    near = 2    // sequential search based on last value
};

enum class InterpMethod
{
    nearest = 0,
    linear = 1,
    next = 2,
    previous = 3
};

enum class ExtrapMethod
{
    clip = 0,
    linear = 1,
    specify = 2
};

enum class TableState
{
    empty = 0,
    size_not_equal = 1,
    size_exceed_limit = 2,
    x_not_mono_increase = 3,
    valid = 4
};

class InterpTable1D
{
public:
    // Constructors and destructor
    InterpTable1D();
    InterpTable1D(const vector<double> &x_table, const vector<double> &y_table);
    ~InterpTable1D() = default;

    // Get table state
    std::size_t size();
    bool valid();
    bool empty();

    // Set and clear the table values
    void SetTableValue(const vector<double> &x_table, const vector<double> &y_table);
    void ClearValue();

    // Lookup table based on input, using current search, interp, and extrap methods
    double LookupTable(double input_value);

    // Configure the methods
    void SetSearchMethod(const SearchMethod& method) { search_method_ = method; }
    void SetInterpMethod(const InterpMethod& method) { interp_method_ = method; }
    void SetExtrapMethod(const ExtrapMethod& method) { extrap_method_ = method; }

private:
    vector<double> x_table_;
    vector<double> y_table_;

    // Methods for checking tables
    void RefreshTableState();
    bool isStrictlyIncreasing(const vector<double>& input_vector);
    //bool CheckTableTypes(const vector<double>& input_vector1, const vector<double>& input_vector2);
    TableState CheckTableState(const vector<double>& input_vector1, const vector<double>& input_vector2);

    // Prelookup to find the index of the input value
    std::size_t PreLookup(double input_value);

    // Interpolation between the two closest points
    double Interpolation(std::size_t prelookup_index, double input_value);

    // Extrapolation if input is out of bounds
    double Extrapolation(double input_value);

    // Currently selected methods
    SearchMethod search_method_ = SearchMethod::bin;
    InterpMethod interp_method_ = InterpMethod::linear;
    ExtrapMethod extrap_method_ = ExtrapMethod::clip;

    // State of table
    bool table_empty_ = true; // indicates the table is empty
    bool table_valid_ = false; // indicates the table is valid
    bool set_value_success_ = false;  // indicates whether a successful SetTableValue has been executed.
    const std::size_t max_table_size_ = 1000000; // do not exceed 1M, though uint32_t can support up to 4294967295U.
    std::size_t table_size_ = 0U; // length of table
};
