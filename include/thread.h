#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include "status.h"

class Thread {
public:
    Thread();
    virtual ~Thread();

    virtual Status Start();
    virtual Status WaitForDeath();
    
private:
    static void* StartFunctionOfThread(Thread *thread_ptr);

private:
    pthread_t thread_id_;
    bool running_;

protected:
    virtual void Run() = 0;
};

#endif //THREAD_H_