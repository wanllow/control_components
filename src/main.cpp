#include <iostream>
#include <algorithm>
#include <Eigen/Dense>
#include "lookup_table1d.h"
#include "lookup_table2d.h"

using std::size_t;
using std::vector;

void TestTable1D()
{
	std::vector<double> x_vec{0, 1, 2, 3, 4, 5, 6, 7};
	std::vector<double> y_vec{0, 1, 4, 9, 9, 4, 1, 0};
	LookupTable1D table_1d(x_vec, y_vec);
	// table_1d.SetSearchMethod(LookupTable::SearchMethod::near);
	// table_1d.SetInterpMethod(LookupTable::InterpMethod::linear);
	// table_1d.SetExtrapMethod(LookupTable::ExtrapMethod::clip);
	std::vector<double> x_value{-0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5};
	for(auto &xval : x_value)
	{
		std::cout << table_1d.Lookup(xval) << "\t";
	}
	std::cout << std::endl;
}

void TestTable2D()
{
	std::vector<double> row_vec = {1.0, 2.0, 3.0, 4.0};
	std::vector<double> col_vec = {10.0, 20.0, 30.0};
	std::vector<double> map_vec = {
		11.0, 12.0, 13.0, // Row 1
		21.0, 22.0, 23.0, // Row 2
		31.0, 32.0, 33.0, // Row 3
		41.0, 42.0, 43.0  // Row 4
	};
	Eigen::RowVectorXd row_axis = Eigen::Map<const Eigen::RowVectorXd>(row_vec.data(), 4);
	Eigen::RowVectorXd col_axis = Eigen::Map<const Eigen::RowVectorXd>(col_vec.data(), 3);
	Eigen::MatrixXd map_matrix = Eigen::Map<const Eigen::MatrixXd>(map_vec.data(), 3, 4).transpose();
	for (int i = 0; i != 4; ++i)
	{
		for (int j = 0; j != 3; ++j)
		{
			std::cout << map_vec[i * 3 + j] << "\t";
			for (int i = 0; i != 5; ++i)
				;
		}
		std::cout << std::endl;
	}
	std::cout << "===============================" << std::endl;
	// LookupTable2D table_2d(row_axis, col_axis, map_matrix);
	LookupTable2D table_2d(row_vec, col_vec, map_vec);
	std::vector<double> row_value = {0.5, 1.5, 2.5, 3.5, 4.5};
	std::vector<double> col_value = {5.0, 15.0, 25.0, 35.0};
	for (auto rval : row_value)
	{
		for (auto cval : col_value)
		{
			std::cout << table_2d.Lookup(rval, cval) << "\t";
		}
		std::cout << std::endl;
	}
}

int main()
{
	TestTable2D();

	return 0;
}
