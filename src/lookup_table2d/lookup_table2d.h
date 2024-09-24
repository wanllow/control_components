#pragma once
#include <vector>
#include "lookup_table_dependencies.h"



class LookupTable2D
{
    private:
    enum class TableState;

    public:
    LookupTable2D();
    LookupTable2D(const std::vector<double> r_axis, const std::vector<double> c_axis, const std::vector<double> );
};