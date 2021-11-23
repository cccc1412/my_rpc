#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include "status.h"

class Thread {
public:
    Thread();
    virtual ~Thread();

    virtual Status Run(void *p_context = 0);
    virtual Status WaitForDeath();
    
private:
    static void* StartFunctionOfThread(void *p_context);

private:
    void *p_context_;
    pthread_t thread_id_;
    bool running_;

};

#endif //THREAD_H_