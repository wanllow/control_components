#include <iostream>
#include "interp_table1d.h"
int main()
{
	vector<double> x_table{1,2,3,4,5};
	vector<double> y_table{2,5,7,3,6};
	InterpTable1D test_table_1d(x_table, y_table);
	std::cout<<(test_table_1d.size())<<std::endl;
	return 0;
}
