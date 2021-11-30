#include "condition_variable.h"

#include <iostream>

#include "mutex.h"
#include "status.h"

ConditionVariable::ConditionVariable() {
  int ret = pthread_cond_init(&condition_variable_, 0);
  if (ret != 0) {
    std::cout << "pthread_cond_init error!" << std::endl;
  }
}

ConditionVariable::~ConditionVariable() {
  int ret = pthread_cond_destroy(&condition_variable_);
  if (ret != 0) {
    std::cout << "pthread_conda_destroy error!" << std::endl;
  }
}

Status ConditionVariable::Wait(Mutex *p_mutex) {
  if (NULL == p_mutex) {
    return Status(-1, 0);
  }
  int ret = pthread_cond_wait(&condition_variable_, &(p_mutex->mutex_));
  if (ret != 0) {
    std::cout << "pthread_cond_wait error!" << std::endl;
    return Status(-1, 0);
  } else {
    return Status(0, 0);
  }
}

Status ConditionVariable::Wakeup() {
  int ret = pthread_cond_signal(&condition_variable_);
  if (ret != 0) {
    std::cout << "pthread_cond_signal error!" << std::endl;
    return Status(-1, 0);
  } else {
    return Status(0, 0);
  }
}

Status ConditionVariable::WakeupAll() {
  int ret = pthread_cond_broadcast(&condition_variable_);
  if (ret != 0) {
    std::cout << "pthread_cond_broadcast error!" << std::endl;
    return Status(-1, 0);
  } else {
    return Status(0, 0);
  }
}