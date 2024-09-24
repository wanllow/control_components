#include <iostream>
#include <algorithm>
#include "lookup_table1d.h"

using std::size_t;
using std::vector;




int main()
{
	std::vector<double> x_table{1, 2, 3, 4, 5};
	std::vector<double> y_table{2, 5, 7, 3, 6};
	LookupTable1D test_table_1d(x_table, y_table);
	test_table_1d.SetInterpMethod(InterpMethod::nearest);
	std::cout << test_table_1d.LookupTable(4.49) << std::endl;
	return 0;
}
