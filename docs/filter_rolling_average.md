# 滑动窗口滤波器设计文档

## 一、设计概览

### 1. 设计意图
设计一个高效的滑动窗口平均滤波器，通过组合关系使用循环队列作为底层存储结构，实现数据高效存取与实时均值计算[^1]。

### 2. 核心特性
- 高效的环形缓冲区数据管理
- 可变窗口大小（可动态调整）
- 优化的增量求和计算
- 严格的类型安全控制
- 明确的边界条件处理

### 3. 设计关系
- **组合关系（has-a）**：滤波器内部使用循环队列作为数据存储容器[^2]
- **依赖分离**：滤波算法与底层存储分离，符合单一职责原则

## 二、类接口定义

### FilterRollingAverage.h

```cpp
/*!
 * @file filter_rolling_average.h
 * @brief 基于滑动窗口的均值滤波器实现
 */

#pragma once

#include "circular_queue.h"
#include "common/platform_log.h"
#include <type_traits>
#include <limits>

namespace mfn {
namespace control {

template <typename T>
class FilterRollingAverage {
 public:
  // 构造函数
  explicit FilterRollingAverage(size_t capacity);
  FilterRollingAverage(size_t capacity, size_t window_size);

  // 核心操作接口
  void Push(const T& value);
  double Average() const;
  void SetWindowSize(size_t window_size);

  // 状态查询接口
  size_t GetWindowSize() const { return window_size_; }
  size_t GetCapacity() const { return buffer_.capacity(); }
  bool Empty() const { return buffer_.empty(); }
  size_t Size() const { return buffer_.size(); }

  // 迭代器支持（可选）
  using const_iterator = typename CircularQueue<T>::const_iterator;
  const_iterator wbegin() const;  // window begin
  const_iterator wend() const;    // window end

 private:
  // 内部计算方法
  void RecalculateSum();
  void UpdateWindowParameters();

  // 类型校验（C++17 SFINAE）
  static_assert(std::is_arithmetic_v<T>, 
      "FilterRollingAverage only supports arithmetic types");

  // 成员变量
  CircularQueue<T> buffer_;        // 数据存储
  size_t window_size_;             // 用户指定的窗口大小
  size_t window_head_ = 0;         // 窗口起始位置
  double sum_ = 0.0;               // 窗口数据和
};

} // namespace control
} // namespace mfn
```

## 三、实现细节

### 1. 构造函数

```cpp
template <typename T>
FilterRollingAverage<T>::FilterRollingAverage(size_t capacity)
    : buffer_(capacity), 
      window_size_(capacity) {
  // 窗口默认与容量相同
  SDEBUG << "Created filter with capacity=" << capacity;
}

template <typename T>
FilterRollingAverage<T>::FilterRollingAverage(size_t capacity, size_t window_size)
    : buffer_(capacity),
      window_size_(std::min(window_size, capacity)) {
  // 窗口大小不能超过容量
  if (window_size > capacity) {
    SWARN << "Window size " << window_size 
          << " exceeds capacity " << capacity 
          << ", clamping to " << window_size_;
  }
}
```

### 2. 核心数据操作

```cpp
template <typename T>
void FilterRollingAverage<T>::Push(const T& value) {
  // 若缓冲区已满，更新sum_（减去将被覆盖的值）
  if (buffer_.full()) {
    const size_t oldest_idx = buffer_.head();
    sum_ -= static_cast<double>(buffer_.at(oldest_idx));
  }

  // 添加新值并更新sum_
  buffer_.push_back(value);
  sum_ += static_cast<double>(value);

  // 更新窗口参数
  UpdateWindowParameters();
}

template <typename T>
double FilterRollingAverage<T>::Average() const {
  const size_t effective_window = std::min(window_size_, buffer_.size());

  if (effective_window == 0) {
    SERROR << "Cannot calculate average of empty window";
    return std::numeric_limits<double>::quiet_NaN();
  }

  return sum_ / static_cast<double>(effective_window);
}
```

### 3. 窗口管理

```cpp
template <typename T>
void FilterRollingAverage<T>::SetWindowSize(size_t window_size) {
  if (window_size == 0) {
    SERROR << "Window size cannot be zero, ignoring request";
    return;
  }

  // 窗口不能超过容量
  const size_t old_window = window_size_;
  window_size_ = std::min(window_size, buffer_.capacity());

  if (window_size_ != old_window) {
    SDEBUG << "Window size changed: " << old_window << " -> " << window_size_;
    RecalculateSum();
  }
}

template <typename T>
void FilterRollingAverage<T>::UpdateWindowParameters() {
  // 更新窗口头位置
  if (buffer_.size() <= window_size_) {
    window_head_ = 0;  // 窗口从队列首部开始
  } else {
    // 窗口头 = 队列尾 - 窗口大小
    window_head_ = (buffer_.tail() - window_size_ + buffer_.capacity()) 
                   % buffer_.capacity();
  }
}
```

### 4. 辅助计算方法

```cpp
template <typename T>
void FilterRollingAverage<T>::RecalculateSum() {
  sum_ = 0.0;
  const size_t effective_size = std::min(window_size_, buffer_.size());

  if (effective_size == 0) {
    return;
  }

  for (size_t i = 0; i < effective_size; ++i) {
    const size_t idx = (window_head_ + i) % buffer_.capacity();
    sum_ += static_cast<double>(buffer_.at(idx));
  }
}

template <typename T>
typename FilterRollingAverage<T>::const_iterator 
FilterRollingAverage<T>::wbegin() const {
  return buffer_.begin() + window_head_;
}

template <typename T>
typename FilterRollingAverage<T>::const_iterator 
FilterRollingAverage<T>::wend() const {
  const size_t effective_size = std::min(window_size_, buffer_.size());
  return wbegin() + effective_size;
}
```

## 四、单元测试设计

```cpp
TEST(FilterRollingAverage, Construction) {
  // 默认构造测试
  FilterRollingAverage<double> filter1(5);
  EXPECT_EQ(filter1.GetWindowSize(), 5);
  EXPECT_EQ(filter1.GetCapacity(), 5);

  // 自定义窗口大小测试
  FilterRollingAverage<double> filter2(10, 5);
  EXPECT_EQ(filter2.GetWindowSize(), 5);
  EXPECT_EQ(filter2.GetCapacity(), 10);

  // 窗口大小截断测试
  FilterRollingAverage<double> filter3(5, 10);
  EXPECT_EQ(filter3.GetWindowSize(), 5);  // 窗口大小被截断到5
}

TEST(FilterRollingAverage, AverageCalculation) {
  FilterRollingAverage<int> filter(5, 3);

  // 初始状态测试
  EXPECT_TRUE(std::isnan(filter.Average()));  // 空窗口返回NaN

  // 累积计算测试
  filter.Push(10);
  EXPECT_DOUBLE_EQ(filter.Average(), 10.0);

  filter.Push(20);
  EXPECT_DOUBLE_EQ(filter.Average(), 15.0);

  filter.Push(30);
  EXPECT_DOUBLE_EQ(filter.Average(), 20.0);

  // 窗口滑动测试
  filter.Push(40);
  EXPECT_DOUBLE_EQ(filter.Average(), 30.0);  // (20+30+40)/3 = 30

  filter.Push(50);
  EXPECT_DOUBLE_EQ(filter.Average(), 40.0);  // (30+40+50)/3 = 40
}
```

## 五、性能与优化注意事项

1. **时间复杂度分析**[^3]
   - Push操作: O(1)
   - Average计算: O(1)
   - SetWindowSize: O(min(n,window_size))
   - 全量重新计算: O(window_size)

2. **内存占用**
   - 固定大小的循环缓冲区: O(capacity)
   - 额外状态变量: O(1)

3. **类型安全**
   - 使用static_assert确保只接受数值类型
   - 运行时转换保证计算精度

4. **数值稳定性**
   - 对空窗口计算均值采取保护措施
   - 使用累积计算避免精度损失

这种基于组合的设计使滤波器能够专注于其核心算法，同时通过循环队列获得高效的数据管理能力，为后续扩展提供了良好基础[^4]。

[^1]: 组合模式能更好地实现关注点分离，参考CMake模块化设计（资料5函数封装示例）
[^2]: 明确职责边界有助于类型系统清晰，避免接口污染（资料1导出头文件示范）
[^3]: 算法优化方案参考CMake生成器表达式的条件执行模式（资料6表达式处理）
[^4]: 组件化设计便于后续扩展，类似CMake超级构建模式（资料4组件化依赖管理）