#include <iostream>
#include "interp_table1d.h"
int main()
{
	std::vector<double> x_table{1,2,3,4,5};
	std::vector<double> y_table{2,5,7,3,6};
	InterpTable1D test_table_1d(x_table, y_table);
	std::cout<<y_table.front()<<"\t"<<y_table.back()<<std::endl;
	return 0;
}
