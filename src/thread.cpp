#include <iostream>
#include "thread.h"
using namespace std;

Thread::Thread() {
    running_ = false;
}

Thread::~Thread() {

}

Status Thread::Start() {
    if(running_) {
        return Status(-1, 0);
    }
    int ret = pthread_create(&thread_id_, 0, (void *(*)(void *))&StartFunctionOfThread,this);
    if(ret != 0){
        cout<<"Thread::Run, phtrea_create error!"<<endl;
    }
    return Status(0, 0);
}

void* Thread::StartFunctionOfThread(Thread *thread_ptr) {
    thread_ptr->running_ = true;
    thread_ptr->Run();
}

Status Thread::WaitForDeath() {
    
}