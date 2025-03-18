/*!
 * @file circular_queue.h
 * @brief 这个文件定义了一个循环队列，用于存储一段时间内的信号值。
 * @copyright Copyright (c) 2023 MainFunction, Inc. 保留所有权利
 *
 * 该循环队列容器作为信号滑动窗口存储机制，主要用于自动驾驶控制算法分析
 * 最近一段时间内的信号数据。容器设计为在full()之后进入正常工作模式，
 * 之前处于"暖机"阶段。正常工作时持续存入新数据并丢弃最旧数据，
 * 实现moving_windows_data_storage效果。
 */
#pragma once

#include <cassert>
#include <iterator>
#include <stdexcept>
#include <vector>

#include "common/platform_log.h"
#include "common_utils.h"

namespace mfn {
namespace controll {
/*!
 * @class CircularQueue
 * @brief 循环队列容器实现
 *
 * 该容器内部实现为环形缓冲区，支持随机访问迭代器，并提供
 * 标准容器常见操作。为确保head_与tail_不指向同一元素，
 * 内部分配比用户请求容量多1个单位的内存。
 *
 * @tparam T 存储元素类型
 */
template <typename T>
class CircularQueue {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T&;

  class Iterator;
  class ConstIterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /*!
   * @brief 构造函数
   * @param capacity 循环队列容量
   *
   * 内部会分配capacity+1的空间，但用户无需考虑这额外的开销
   */
  explicit CircularQueue(const std::size_t capacity)
      : capacity_(std::min(capacity, max_capacity_) +
                  1),  // 分配比请求容量多1的空间
        head_(0),
        tail_(0),
        size_(0),
        buffer_(std::clamp(capacity, 1, max_capacity_) + 1) {
    if (capacity < 1) {
      SERROR << "循环队列至少有1个元素";

    } else if (capacity > max_capacity_) {
      SERROR << "输入长度超出循环队列容忍范围:" << capacity;
    }
  }
  explicit CircularQueue(const std::size_t capacity,
                         const T& init_value)  // 带初始值的构造
      : capacity_(std::min(capacity, max_capacity_) +
                  1),  // 分配比请求容量多1的空间
        head_(0),
        tail_(0),
        size_(0),
        buffer_(std::clamp(capacity, 1, max_capacity_) + 1, init_value) {
    if (capacity < 1) {
      SERROR << "循环队列至少有1个元素";

    } else if (capacity > max_capacity_) {
      SERROR << "输入长度超出循环队列容忍范围:" << capacity;
    }
  }

  // 禁止拷贝和移动
  CircularQueue(const CircularQueue&) = delete;  // 禁止拷贝构造
  CircularQueue& operator=(const CircularQueue&) = delete;  // 禁止拷贝赋值
  CircularQueue(CircularQueue&&) = delete;  // 禁止移动构造
  CircularQueue& operator=(const CircularQueue&&) = delete;  // 禁止移动赋值

  // 迭代器入口
  iterator begin() noexcept { return iterator(*this, 0); }
  iterator end() noexcept { return iterator(*this, size_); }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_iterator cbegin() const noexcept { return const_iterator(*this, 0); }
  const_iterator cend() const noexcept { return const_iterator(*this, size_); }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  /*!
   * @brief 获取队列设计容量
   * @return 队列可存储的元素数量
   */
  size_type capacity() const noexcept {
    return capacity_ - 1;
  }  // 用户看到的容量比实际分配少1

  /*!
   * @brief 获取队列当前大小
   * @return 队列中的元素数量
   */
  size_type size() const noexcept { return size_; }

  /*!
   * @brief 判断队列是否为空
   * @return 队列为空返回true，否则返回false
   */
  bool empty() const noexcept { return size_ == 0; }

  /*!
   * @brief 判断队列是否已满
   * @return 队列已满返回true，否则返回false
   *
   * 满队列判断条件修改为tail_的下一位置等于head_
   */
  bool full() const noexcept { return size_ == capacity_ - 1; }

  /*!
   * @brief 将新数据拷贝送入队尾
   * @param value 拷贝入队的值
   * @return 操作后队列是否已满
   *
   * 如果队列已满，会覆盖最早的数据
   */
  bool push_back(const reference value) noexcept {
    // 队列满时，head_前进一位
    if (full()) {
      buffer_[tail_] = value;
      tail_ = LimitIndex(tail_ + 1);
      head_ = LimitIndex(head_ + 1);
    } else {
      // 队列不满时，增加一个元素
      buffer_[tail_] = value;
      tail_ = LimitIndex(tail_ + 1);
      ++size_;
    }

    return full();  // 返回操作后队列是否已满
  }

  /*!
   * @brief 将新数据移动送入队尾
   * @param value 移动入队的值
   * @return 操作后队列是否已满
   */
  bool push_back(value_type&& value) noexcept {
    if (full()) {
      buffer_[tail_] = std::move(value);
      tail_ = LimitIndex(tail_ + 1);
      head_ = LimitIndex(head_ + 1);
    } else {
      buffer_[tail_] = std::move(value);
      tail_ = LimitIndex(tail_ + 1);

      ++size_;
    }
    return full();
  }

  /*!
   * @brief 重载<<运算符实现push_back
   * @param value 拷贝入队的值
   * @return 队列自身引用
   */
  CircularQueue& operator<<(const reference value) {
    push_back(value);
    return *this;
  }

  /*!
   * @brief 重载<<运算符实现push_back（右值版本）
   * @param value 移动入队的值
   * @return 队列自身引用
   */
  CircularQueue& operator<<(value_type&& value) {
    push_back(std::move(value));
    return *this;
  }

  /*!
   * @brief 将队首数据弹出
   * @return 操作成功返回true，队列为空返回false
   */
  bool pop() noexcept {
    if (empty()) return false;
    head_ = LimitIndex(head_ + 1);
    --size_;
    return true;
  }

  /*!
   * @brief 验证队列状态是否正确
   *
   * 仅在调试模式下进行断言检查
   */
  void ValidateState() const {
#ifndef NDEBUG
    assert(head_ < capacity_);
    assert(tail_ < capacity_);
    assert(size_ <= (capacity_ - 1));
#endif
  }

  /*!
   * @brief 随机访问队列元素
   * @param index 元素索引，从0开始
   * @return 对应位置元素的引用
   */
  reference operator[](std::size_t index) noexcept {
    if (index >= size_) {
      SERROR << "索引超出范围：" << index << "，队列大小：" << size_;
    }
    return buffer_[LimitIndex(head_ + index)];
  }

  /*!
   * @brief 随机访问队列元素（const版本）
   * @param index 元素索引，从0开始
   * @return 对应位置元素的常量引用
   */
  const reference operator[](size_type index) const noexcept {
    if (index >= size_) {
      SERROR << "索引超出范围：" << index << "，队列大小：" << size_;
    }
    return buffer_[LimitIndex(head_ + index)];
  }

  /*!
   * @brief 获取队首元素
   * @return 队首元素的引用
   */
  reference front() {
    if (empty()) {
      SERROR << "试图访问空队列的队首元素";
      return buffer_[0];  // 返回首元素位置避免未定义行为
    }
    return buffer_[head_];
  }

  /*!
   * @brief 获取队尾元素
   * @return 队尾元素的引用
   */
  reference back() {
    if (empty()) {
      SERROR << "试图访问空队列的队尾元素";
      return buffer_[0];  // 返回首元素位置避免未定义行为
    }
    return buffer_[tail_ == 0 ? capacity_ - 1 : tail_ - 1];
  }

  /*!
   * @brief 清空队列
   */
  void Clear() {
    head_ = 0;
    tail_ = 0;
    size_ = 0;
  }

 protected:
  /*!
   * @brief 限制坐标超出容量
   */
  inline std::size_t LimitIndex(const std::size_t index) {
    if (index <= capacity_) {
      return index;
    } else if (index <= capacity_ * 2) {
      return index - capacity_;
    } else {
      return index % capacity_;
    }
  }

  /*!
   * @brief 最大允许容量
   */
  static constexpr std::size_t max_capacity_ = 100000;  // 不得大于十万
  /*!
   * @brief 队列的实际容量（比用户请求多1）
   */
  const std::size_t capacity_;

  /*!
   * @brief 队列头索引
   */
  std::size_t head_;

  /*!
   * @brief 下一个可写位置
   */
  std::size_t tail_;

  /*!
   * @brief 当前元素数量
   */
  std::size_t size_;

  /*!
   * @brief 存储容器
   */
  std::vector<value_type> buffer_;

 public:  // 迭代器要声明为public
  /*!
   * @class Iterator
   * @brief 随机访问迭代器实现
   */
  class Iterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

   private:
    Iterator(CircularQueue& queue, size_type pos) : queue_(queue), pos_(pos) {}
    friend class CircularQueue<value_type>;

   public:
    // 核心访问操作
    reference operator*() const {
#ifndef NDEBUG
      assert(pos_ < queue_.size() && "解引用无效迭代器");
#endif
      return queue_[pos_];
    }

    pointer operator->() const { return &(operator*()); }

    // 迭代器自增
    Iterator& operator++() {
      ++pos_;
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++pos_;
      return tmp;
    }
    // 迭代器自减
    Iterator& operator--() {
      --pos_;
      return *this;
    }
    Iterator operator--(int) {
      Iterator tmp = *this;
      --pos_;
      return tmp;
    }

    // 随机访问支持
    Iterator operator+(difference_type n) const {
      return Iterator(queue_, pos_ + n);
    }
    Iterator operator-(difference_type n) const {
      return Iterator(queue_, pos_ - n);
    }

    difference_type operator-(const Iterator& other) const {
      return pos_ - other.pos_;
    }

    // 比较操作
    bool operator==(const Iterator& other) const {
      return &queue_ == &other.queue_ && pos_ == other.pos_;
    }
    bool operator!=(const Iterator& other) const { return !(*this == other); }
    bool operator<(const Iterator& other) const { return pos_ < other.pos_; }
    bool operator>(const Iterator& other) const { return other < *this; }
    bool operator<=(const Iterator& other) const { return !(other < *this); }
    bool operator>=(const Iterator& other) const { return !(*this < other); }

   private:
    CircularQueue& queue_;
    size_type pos_;
  };

  /*!
   * @class ConstIterator
   * @brief 常量随机访问迭代器实现
   */
  class ConstIterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

   private:
    ConstIterator(const CircularQueue& queue, size_type pos)
        : queue_(queue), pos_(pos) {}

    friend class CircularQueue<value_type>;

   public:
    // 核心访问操作
    reference operator*() const {
#ifndef NDEBUG
      assert(pos_ < queue_.size() && "解引用无效迭代器");
#endif
      return queue_[pos_];
    }

    pointer operator->() const { return &(operator*()); }

    // 迭代器自增
    ConstIterator& operator++() {
      ++pos_;
      return *this;
    }
    ConstIterator operator++(int) {
      ConstIterator tmp = *this;
      ++pos_;
      return tmp;
    }
    // 迭代器自减
    ConstIterator& operator--() {
      --pos_;
      return *this;
    }
    ConstIterator operator--(int) {
      ConstIterator tmp = *this;
      --pos_;
      return tmp;
    }

    // 随机访问支持
    ConstIterator operator+(difference_type n) const {
      return ConstIterator(queue_, pos_ + n);
    }
    ConstIterator operator-(difference_type n) const {
      return ConstIterator(queue_, pos_ - n);
    }

    difference_type operator-(const ConstIterator& other) const {
      return pos_ - other.pos_;
    }

    // 比较操作
    bool operator==(const ConstIterator& other) const {
      return &queue_ == &other.queue_ && pos_ == other.pos_;
    }
    bool operator!=(const ConstIterator& other) const {
      return !(*this == other);
    }
    bool operator<(const ConstIterator& other) const {
      return pos_ < other.pos_;
    }
    bool operator>(const ConstIterator& other) const { return other < *this; }
    bool operator<=(const ConstIterator& other) const {
      return !(other < *this);
    }
    bool operator>=(const ConstIterator& other) const {
      return !(*this < other);
    }

   private:
    const CircularQueue& queue_;
    size_type pos_;
  };
};
}  // namespace controll
}  // namespace mfn
