/*
    这个文档利用CircularQueue容器设计了一个滚动均值滤波器，虽然循环队列的容量不可变，但是可以利用队列的部分容量虚拟出
    动态变化的窗口，在队列头指针和尾指针间嵌入一个window_head_的指针，只统计window_head_与tail_之间的数据，实现了
    动态窗口的平均值滤波。
    为了高效地计算累加值，采用增量式算法，即只有当push_back和pop的时候采用sum +=
   push_value   sum -= pop_value 的方法高效处理推入弹出时求和
    对于窗口变化引起的sum_变化要分而论之：如果变化量较小，即delta_windows_size <
   new_window_size，则遍历变化元
    素，累加后修正到sum_上去；若变化量较大，delta_window_size >
   new_window_size，则遍历window累加。由于这种算法
    资源消耗较高，因此要求用户在输入窗口长度前务必滤波加限制变化率，以防止计算资源的空耗。

*/

#pragma once

#include "circular_queue.h"
#include "common/platform_log.h"

namespace mfn {
namespace control {
class FilterRollingAverage{
    public:

    private:
    CircularQueue buffer_;
};
}  // namespace control
}  // namespace mfn