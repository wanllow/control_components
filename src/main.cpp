#include <iostream>
#include <algorithm>
#include <Eigen/Dense>
#include "lookup_table1d.h"

using std::size_t;
using std::vector;

int main()
{
	// std::vector<double> x_vec{1, 2, 3, 4, 5};
	// std::vector<double> y_vec{2, 5, 7, 3, 6};
	// Eigen::VectorXd x_table(x_vec.begin(), x_vec.end());
	// // Eigen::VectorXd y_table(y_vec.begin(), y_vec.end());
	Eigen::VectorXd x_table(5), y_table(5);
	x_table << 1, 2, 3, 4, 5;  
    y_table << 2, 5, 7, 3, 6;  
	LookupTable1D test_table_1d(x_table, y_table);
	test_table_1d.SetInterpMethod(InterpMethod::linear);
	test_table_1d.SetExtrapMethod(ExtrapMethod::clip);
	std::cout << "The result is: ";
	std::cout << test_table_1d.LookupTable(2.5) << std::endl;
	return 0;
}
