#ifndef THREAD_QUEUE_H_
#define THREAD_QUEUE_H_

#include <deque>

template<typename T, typename D = std::deque<T> >
class ThreadQueue {
public:
    typedef D queue_type;
    bool pop_front(T& t);
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
};

template<typename T, typename D>
bool ThreadQueue<T, D>::pop_front(T& t) {
    t = queue_.front();
    queue_.pop_front();
    size_--;
    return true;
}

template<typename T, typename D>
void ThreadQueue<T, D>::push_back(T& t) {
    queue_.push_back(t);
    size_++;
}

template<typename T, typename D>
void ThreadQueue<T, D>::push_front(T& t) {
    queue_.push_front();
    size_++;
}

template<typename T, typename D>
void ThreadQueue<T, D>::clear() {
    queue_.clear();
    size_ = 0;
}

template<typename T, typename D>
bool ThreadQueue<T, D>::empty() {
    return queue_.empty();
}

template<typename T, typename D>
auto ThreadQueue<T, D>::begin() {
    return queue_.begin();
}

template<typename T, typename D>
auto ThreadQueue<T, D>::end() {
    return queue_.end();
}

template<typename T, typename D>
void ThreadQueue<T, D>::swap(D &q) {
    q.swap(queue_);//交换两个队列的 内容
}

#endif //THREAD_QUEUE_H_