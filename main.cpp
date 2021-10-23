#include <iostream>
#include "epoller.h"
#include "epoller_server.h"
int main() {
    NetThread* net_thread = new NetThread();
    string ip = "127.0.0.1";
    int port = 9996;
    net_thread->bind(ip,port);
    net_thread->createEpoll();
    net_thread->run();
    
    delete net_thread;
    return 0;
}