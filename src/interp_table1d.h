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
    seq = 0,
    bin = 1
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


class InterpTable1D
{
public:
    // Constructors and destructor
    InterpTable1D();
    InterpTable1D(const vector<double> &x_table, const vector<double> &y_table);
    ~InterpTable1D() = default;

    // Set and clear the table values
    void SetValue(const vector<double> &x_table, const vector<double> &y_table);
    void ClearValue();

    // Lookup table based on input, using current search, interp, and extrap methods
    double LookupTable(double input_value);

    // Configure the methods
    void SetSearchMethod(SearchMethod method) { search_method_ = method; }
    void SetInterpMethod(InterpMethod method) { interp_method_ = method; }
    void SetExtrapMethod(ExtrapMethod method) { extrap_method_ = method; }

private:
    vector<double> x_table_;
    vector<double> y_table_;

    // Methods for checking and setting tables
    std::size_t GetTableSize(const vector<double>& input_vector1, const vector<double>& input_vector2);
    std::size_t GetTableSize();
    void CheckTableSize();
    void SetTableX(const vector<double> &x_table);
    void SetTableY(const vector<double> &y_table);

    // Prelookup to find the index of the input value
    uint32_t PreLookup(double input_value);

    // Interpolation between the two closest points
    double Interpolation(uint32_t prelookup_index, double input_value);

    // Extrapolation if input is out of bounds
    double Extrapolation(double input_value);

    // Currently selected methods
    SearchMethod search_method_ = SearchMethod::bin;
    InterpMethod interp_method_ = InterpMethod::linear;
    ExtrapMethod extrap_method_ = ExtrapMethod::clip;

    // State of table: true for valie and false for invalid
    bool table_state_ = false;
    const uint32_t max_table_size_ = 4294967295U;
};
