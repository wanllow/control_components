#include <iostream>
#include <algorithm>
#include "interp_table1d.h"

using std::size_t;
using std::vector;




int main()
{
	std::vector<double> x_table{1, 2, 3, 4, 5};
	std::vector<double> y_table{2, 5, 7, 3, 6};
	InterpTable1D test_table_1d(x_table, y_table);
	//std::size_t index = SearchIndexNear(4.5, x_table, 5);
	std::cout << test_table_1d.size() << std::endl;
	return 0;
}
