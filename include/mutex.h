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

private:
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

private:
    pthread_mutex_t mutex_;
};

#endif //MUTEX_H_