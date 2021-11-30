#ifndef THREAD_QUEUE_H_
#define THREAD_QUEUE_H_

#include <assert.h>

#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
//#include "monitor.h"

template <typename T, typename D = std::deque<T> >
class ThreadQueue {
 public:
  typedef D queue_type;
  bool pop_front(T& t, size_t millsecond);
  void push_back(T& t);
  void push_front(T& t);
  void swap(queue_type& q);
  void clear();
  bool empty();
  auto begin();
  auto end();

 protected:
  queue_type queue_;
  size_t size_;

 private:
  std::mutex mutex_;
  std::condition_variable cond_;
};

template <typename T, typename D>
bool ThreadQueue<T, D>::pop_front(T& t, size_t millsecond) {
  std::unique_lock<std::mutex> lock(mutex_);
  while (queue_.empty()) {  //为什么if不行？
    cond_.wait(lock);
  }
  assert(!queue_.empty());
  t = queue_.front();
  queue_.pop_front();
  size_--;
  return true;
}

template <typename T, typename D>
void ThreadQueue<T, D>::push_back(T& t) {
  std::unique_lock<std::mutex> lock(mutex_);
  queue_.push_back(t);
  size_++;
  cond_.notify_one();
}

template <typename T, typename D>
void ThreadQueue<T, D>::push_front(T& t) {
  std::unique_lock<std::mutex> lock(mutex_);
  queue_.push_front(t);
  size_++;
  cond_.notify_one();
}

template <typename T, typename D>
void ThreadQueue<T, D>::clear() {
  std::unique_lock<std::mutex> lock(mutex_);
  queue_.clear();
  size_ = 0;
}

template <typename T, typename D>
bool ThreadQueue<T, D>::empty() {
  return queue_.empty();
}

template <typename T, typename D>
auto ThreadQueue<T, D>::begin() {
  std::unique_lock<std::mutex> lock(mutex_);
  return queue_.begin();
}

template <typename T, typename D>
auto ThreadQueue<T, D>::end() {
  std::unique_lock<std::mutex> lock(mutex_);
  return queue_.end();
}

template <typename T, typename D>
void ThreadQueue<T, D>::swap(D& q) {
  std::unique_lock<std::mutex> lock(mutex_);
  // if(queue_.empty()) {
  //     cond_.wait(lock);
  // }
  q.swap(queue_);  //交换两个队列的 内容
}

#endif  // THREAD_QUEUE_H_