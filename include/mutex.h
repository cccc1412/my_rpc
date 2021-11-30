#ifndef MUTEX_H_
#define MUTEX_H_
#include <pthread.h>

#include "status.h"

class Mutex {
 public:
  friend class ConditionVariable;

  Mutex();
  virtual ~Mutex();

  Status Lock();
  Status Unlock();

 protected:
  Mutex(const Mutex&);
  Mutex& operator=(const Mutex&);

 protected:
  pthread_mutex_t mutex_;
};

#endif  // MUTEX_H_