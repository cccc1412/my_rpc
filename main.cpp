#include <iostream>
#include "epoller.h"
#include "epoller_server.h"
int main() {
    EpollerServer *epoll_server = new EpollerServer();    
    NetThread* net_thread = new NetThread();
    epoll_server->SetNetThread(net_thread);
    string ip = "127.0.0.1";
    int port = 9996;
    net_thread->bind(ip,port);
    net_thread->createEpoll();
    Handle handle;
    handle.SetEpollServer(epoll_server);
    handle.Start();
    net_thread->run();
    //delete net_thread;
    return 0;
}