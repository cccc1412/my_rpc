#include <iostream>
#include "thread.h"
using namespace std;

Thread::Thread() {

}

Thread::~Thread() {

}

Status Thread::Run(void *p_context) {
    if(running_) {
        return Status(-1, 0);
    }
    int ret = pthread_create(&thread_id_, 0, StartFunctionOfThread, this);
    if(ret != 0){
        cout<<"Thread::Run, phtrea_create error!"<<endl;
    }
    return Status(0, 0);
}

void* Thread::StartFunctionOfThread(void *p_this) {
    Thread *p_thread_this = (Thread *)p_this;
    void *p_context_ = p_thread_this->p_context_;
    p_thread_this->p_context_ = NULL;
}

Status Thread::WaitForDeath() {
    
}