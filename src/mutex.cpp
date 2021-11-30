#include "mutex.h"

#include <iostream>

Mutex::Mutex() {
  int ret = pthread_mutex_init(&mutex_, 0);
  if (ret != 0) {
    std::cout << "pthread_mutex_init error!" << std::endl;
  }
}

Mutex::~Mutex() {
  int ret = pthread_mutex_destroy(&mutex_);
  if (ret != 0) {
    std::cout << "pthread_mutex_destroy error!" << std::endl;
  }
}

Status Mutex::Lock() {
  int ret = pthread_mutex_lock(&mutex_);
  if (ret != 0) {
    std::cout << "pthread_mutex_lock error!" << std::endl;
    return Status(-1, 0);
  } else {
    return Status(0, 0);
  }
}

Status Mutex::Unlock() {
  int ret = pthread_mutex_unlock(&mutex_);
  if (ret != 0) {
    std::cout << "pthread_mutex_unlock error!" << std::endl;
    return Status(-1, 0);
  } else {
    return Status(0, 0);
  }
}