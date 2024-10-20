#pragma once
#include <vector>
#include <limits>
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

    enum class SetState
    {
        fail = 0,   // new value not set, old value invalid
        remain = 1, // new value not set, old value valid
        success = 2 // new value set successfully
    };


    // Constructors and destructors
    LookupTable() = default;
    virtual ~LookupTable() = default;

    // Get present state
    bool valid() const { return table_valid_; }
    bool empty() const { return table_empty_; }

    // Set methods for search, interpolation, and extrapolation
    void SetSearchMethod(const SearchMethod &method) { search_method_ = method; }
    void SetInterpMethod(const InterpMethod &method) { interp_method_ = method; }
    virtual void SetExtrapMethod(const ExtrapMethod &method) { extrap_method_ = method; }
    void SetEpsilon(const double &epsilon) { epsilon_ = epsilon > 0 ? epsilon : epsilon_; }

    // virtual void SetTable() = 0;   // SetTable without any input parameter, it's complete virtual function
    virtual bool ClearTable() = 0; // ClearTable may be different for 1dTable and 2dTable

    // Three main search index functions
    std::size_t SearchIndex(const double &value, const Eigen::RowVectorXd &table, const SearchMethod &method, const std::size_t &last_index = 1);


protected:
    
    // Common member variables for table configuration
    bool table_empty_ = true;
    bool table_valid_ = false;

    std::size_t last_index_ = 1;    // 0 is out of range, so default value is 1.

    SearchMethod search_method_ = SearchMethod::bin;
    InterpMethod interp_method_ = InterpMethod::linear;
    ExtrapMethod extrap_method_ = ExtrapMethod::clip;

    // important threthold
    double epsilon_ = std::numeric_limits<double>::epsilon();
    const std::size_t max_table_size_ = 1000000U; // do not exceed 1M, uint32_t can support up to 4294967295U.

    bool isStrictlyIncreasing(const Eigen::RowVectorXd &input_vector);
};