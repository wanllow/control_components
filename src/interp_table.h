#pragma once
#include <array>
#include <vector>

class InterpTable
{
public:
    std::vector<double> prelookup_table_;
    std::vector<double> table_value_;
};