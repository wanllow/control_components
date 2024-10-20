#include <iostream>
#include <algorithm>
#include <Eigen/Dense>
#include "lookup_table1d.h"

using std::size_t;
using std::vector;

int main()
{
	std::vector<double> x_vec{0, 1, 2, 3, 4, 5, 6, 7};
	std::vector<double> y_vec{0, 1, 4, 9, 9, 4, 1, 0};
	// Eigen::RowVectorXd x_table(x_vec.data(), x_vec.size());
	// Eigen::RowVectorXd y_table(y_vec.data(), y_vec.size());
	Eigen::RowVectorXd x_table(5), y_table(5);
	x_table << 1, 2, 3, 4, 5;
	y_table << 2, 5, 7, 3, 6;
	LookupTable1D test_table_1d(x_vec, y_vec);
	test_table_1d.SetSearchMethod(LookupTable::SearchMethod::near);
	test_table_1d.SetInterpMethod(LookupTable::InterpMethod::linear);
	test_table_1d.SetExtrapMethod(LookupTable::ExtrapMethod::clip);
	std::cout << "The result is: ";
	std::cout << test_table_1d.LookupTable(8) << std::endl;
	// std::cout << test_table_1d.SearchIndex(0.5, x_table, LookupTable::SearchMethod::near, 5.1) << std::endl;
	return 0;
}
