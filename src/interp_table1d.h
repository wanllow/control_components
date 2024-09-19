#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <stdexcept>

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
    // Constructors for different table types
    InterpTable1D();
    InterpTable1D(const std::vector<double>& prelookup_table, const std::vector<double>& table_value);
    
    // Template constructor for std::array
    template <std::size_t N>
    InterpTable1D(const std::array<double, N>& prelookup_table, const std::array<double, N>& table_value) {
        prelookup_table_.assign(prelookup_table.begin(), prelookup_table.end());
        table_value_.assign(table_value.begin(), table_value.end());
        CheckTableSize();
    }

    // Constructor for traditional arrays
    InterpTable1D(const double* prelookup_table, const double* table_value, uint32_t table_size);

    ~InterpTable1D() = default;

    // Set the table values after initialization
    void SetValue(const std::vector<double>& prelookup_table, const std::vector<double>& table_value);

    // Lookup table based on input, using current search, interp, and extrap methods
    double LookupTable(double input_value);

    // Configure the methods
    void SetSearchMethod(SearchMethod method) { search_method_ = method; }
    void SetInterpMethod(InterpMethod method) { interp_method_ = method; }
    void SetExtrapMethod(ExtrapMethod method) { extrap_method_ = method; }

private:
    std::vector<double> prelookup_table_;
    std::vector<double> table_value_;

    // Methods for checking and setting tables
    void CheckTableSize();
    void SetPreTable(const std::vector<double>& prelookup_table);
    void SetTable(const std::vector<double>& table_value);

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
};
