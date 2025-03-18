# CircularQueue 用户说明文档

## 一、类概述
`mfn::control::CircularQueue` 是为实时自动控制设计的高性能循环队列容器[^1]，主要用于存储固定时间窗口的信号数据（如传感器采样序列、控制指令缓存等）。通过预分配内存和优化索引计算，可在实时控制系统中实现O(1)时间复杂度的插入/删除操作。

### 循环队列内存布局（容量=11，实际存储10元素）

```text
队列状态：未满（Tail+1 != Head）
当前元素数：8/10

索引 | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 
-----+----+----+----+----+----+----+----+----+----+----+----
数据 | A  | B  |    | C  | D  | E  | F  | G  | H  |    | I
    |    |    | ↑  | ↑  |    |    |    |    |    |    |  
    |    |    |Tail|Head|    |    |    |    |  
```

### 关键特性说明：
1. **容量设计**：总空间11个元素，实际存储10个有效数据[^1]
2. **指针逻辑**：
   - Head指向最早元素（索引2）
   - Tail指向最新元素的下一个空位（索引3）
3. **满队列示例**：
```text
当队列填满时：
索引 | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 
-----+----+----+----+----+----+----+----+----+----+----+----
数据 | A  | B  | C  | D  | E  | F  | G  | H  | I  | J  |  
    | ↑  |    |    |    |    |    |    |    |    |    | ↑
    |Head|    |    |    |    |    |    |    |    |    |Tail
```

[^1]: 内存管理参考CMake动态数组策略（资料1内存分配示例）
[^2]: 索引计算采用与车辆动力学模型相同的边界检查方法（预设集成规范）

## 二、核心特性
### 1. 高效状态判断
- **空满状态**：通过头尾指针间距判断，仅在`full()`方法使用模运算[^2]
- **索引优化**：通过边界检查替代模运算（示例：当`tail_=1024`时自动归零）
```cpp
// 模运算优化实现
void push_back(const T& value) {
    if (tail_ >= capacity_) {  // 替代 tail_ % capacity_
        tail_ = 0; 
    }
}
```

### 2. 安全容量管理
```cpp
// 创建容量为1000的队列（实际存储999元素）
CircularQueue<double> signal_buffer(1000); 

// 动态扩容（旧数据迁移）
signal_buffer.resize(2000);  // 触发SWARN日志若数据丢失[^3]
```

### 3. 实时日志支持
```cpp
q.push_back(data);  // 容量>90%时触发SDEBUG预警
q.pop_front();      // 空队列操作触发SERROR日志
```

## 三、快速上手
### 实例化
```cpp
// 创建固定容量的循环队列（实际可存储数量为capacity-1）
#include "base/circular_queue.h"

// 创建容量为1000的队列（实际可存储999个元素）
mfn::control::CircularQueue<double> signal_buffer(1000);

// 创建存储Eigen向量的队列
mfn::control::CircularQueue<Eigen::Vector3d> trajectory_points(100);
```
### 基本操作
```cpp
// 添加元素
signal_buffer.push_back(sensor_value);

// 获取队首元素
double oldest_value = signal_buffer.front();

// 移除队首元素
signal_buffer.pop_front();

// 检查队列状态
if (signal_buffer.empty()) {
  SWARN << "No sensor data available";
}

if (signal_buffer.full()) {
  SDEBUG << "Signal buffer at capacity, oldest data will be overwritten";
}

// 获取当前元素数量和容量
size_t used = signal_buffer.size();
size_t capacity = signal_buffer.capacity();
```
### 迭代器遍历元素
```cpp
// 遍历所有元素（从最旧到最新）
for (auto& member : signal_buffer) {
  std::cout << member <<'\t' ;
    member = data; // data用户可自己定义
  // 处理value...
}

// 清空队列
signal_buffer.clear();
```
### 小型完整示例
```cpp
#include "base/circular_queue.h"

int main() {
    // 创建容量为5的队列（实际存储4元素）
    mfn::control::CircularQueue<int> queue(5);
  
    // 填充数据
    for(int i=1; i<=4; ++i){
        queue.push_back(i*10);  // 10,20,30,40
    }
  
    // 遍历访问
    while(!queue.empty()){
        SINFO << "Current front: " << queue.front();
        queue.pop_front();
    }
  
    return 0;
}
```

### 异常处理示例
```cpp
try {
    queue.front();  // 空队列访问触发异常
} catch (const std::runtime_error& e) {
    SWARN << "Queue operation failed: " << e.what();
    // 执行恢复逻辑...
}
```

## 四、设计原理及算法效率

### 1. 内存管理

- **预分配固定内存**：构造时一次性分配所有内存，避免运行时动态分配[^2]
- **循环复用内存**：通过头尾指针管理，在固定大小内存块上循环存储数据
- **禁止拷贝/移动**：防止意外的深拷贝操作，确保实时性和资源控制[^3]

### 2. 性能优化

- **常数时间操作**：所有基本操作（push_back、pop_front、front、back）均为O(1)时间复杂度
- **避免模运算**：使用条件判断替代昂贵的模运算，提高索引计算效率
- **边界检查优化**：针对临界状态（空/满）采用特殊优化，减少不必要的计算

### 3. 安全设计

- **防溢出保护**：满队列时拒绝新数据，通过日志提示容量问题
- **边界检查**：所有访问操作都进行边界验证，防止越界访问
- **日志跟踪**：关键操作失败时生成警告或错误日志，便于问题诊断


| 方法          | 时间复杂度 | 说明                                                                 |
|---------------|------------|----------------------------------------------------------------------|
| `push_back`   | O(1)       | 队尾插入元素，满队列时抛出异常并记录SERROR日志                       |
| `pop_front`   | O(1)       | 队首删除元素，空队列时抛出异常并记录SERROR日志                       |
| `resize`      | O(n)       | 迁移数据时保留旧元素顺序，新容量不足时触发SWARN警告                   |
| `front`       | O(1)       | 获取队首元素引用，空队列时抛出异常                                   |

## 五、注意事项
1. **线程安全**：本实现未内置锁机制，多线程场景需外部同步[^4]
2. **容量限制**：构造函数参数需≥2，实际存储量为`capacity-1`
3. **性能建议**：优先预分配足够容量，运行时不可resize
4. **禁用拷贝移动**：设计为静态资源，不支持整体复制或转移


[^1]: 实时控制系统要求确定性行为和低延迟，此容器特别优化了这些特性
[^2]: 模运算的耗时是加减法的25倍以上，是乘法运算的20倍以上，应当避免高频求模运算
[^3]: 禁用拷贝/移动操作避免了隐式的大块内存复制，确保高效运行
[^4]: 本容器应该置于自动驾驶控制框架内部，在静态线程上运行，防止线程竞争