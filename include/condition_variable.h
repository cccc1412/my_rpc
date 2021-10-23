#ifndef CONDITION_VARIABLE_H_
#define CONDITION_VARIABLE_H_

#include <pthread.h>
#include "status.h"
#include "mutex.h"

class ConditionVariable {
public:
    ConditionVariable();
    virtual ~ConditionVariable();

    Status Wait(Mutex *mutex);
    Status Wakeup();
    Status WakeupAll();

private:
    ConditionVariable(const ConditionVariable&);
    ConditionVariable& operator=(const ConditionVariable&);

private:
    pthread_cond_t condition_variable_;
};

#endif //CONDITION_VARIABLE_H_