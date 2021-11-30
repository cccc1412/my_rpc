#ifndef MONITOR_H_
#define MONITOR_H_
#include <mutex>
#include <condition_variable>
//#include "condition_variable.h"

class Monitor {
 public:
  Monitor():nnotify_(0) {}
  virtual ~Monitor() {}
  void Lock() {
    mutex_.lock();
    nnotify_ = 0;
  } 

  void Unlock() {
    notifyImpl(nnotify_);
    mutex_.unlock();
  }

  void Wait() {
    notifyImpl(nnotify_);
    try {
      std::unique_lock <std::mutex> lock(mutex_);
      cond_.wait(lock);
    }
    catch(...) {
      nnotify_ = 0;
      throw;
    }
    nnotify_ = 0;
  }

  void notify() {
    if(nnotify_ != -1) {
      nnotify_++;
    }
  }

  void notifyAll() {
    nnotify_ = -1;
  }

 protected:
  void notifyImpl(int nnotify) {
    if(nnotify != 0) {
      if(nnotify == -1) {
          cond_.notify_all();
          return;
      }else {
        while(nnotify > 0) {
          cond_.notify_one();
          nnotify--;
        }
      }
    }
  }

 protected:
  int nnotify_;//等待在这个条件变量上的个数？
  std::condition_variable cond_;
  std::mutex mutex_;
};

#endif //MONITOR_H_