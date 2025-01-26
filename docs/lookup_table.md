# LookupTable2D 使用指南

## 数据格式说明

### 1. 行轴与列轴
- **行轴 (`row_axis`)**  
  严格递增的一维数据，对应矩阵的行（第0维）。  
  例如：`row_axis = [100, 200, 300]` 表示矩阵有3行。

- **列轴 (`col_axis`)**  
  严格递增的一维数据，对应矩阵的列（第1维）。  
  例如：`col_axis = [10, 20]` 表示矩阵有2列。

### 2. 映射矩阵 (`map_matrix`)
- **Eigen矩阵输入**  
  直接使用`Eigen::MatrixXd`类型，维度必须为 `[row_axis.size() × col_axis.size()]`。  
  示例：
  ```cpp
  Eigen::RowVectorXd row_axis(3); row_axis << 100, 200, 300;
  Eigen::RowVectorXd col_axis(2); col_axis << 10, 20;
  Eigen::MatrixXd map_matrix(3, 2);
  map_matrix << 5.0, 6.0,
                7.0, 8.0,
                9.0, 10.0;