#ifndef EPOLLER_SERVER_H_
#define EPOLLER_SERVER_H

#include "epoller.h"
#include <string>
#include <iostream>
#include <netinet/in.h>

using namespace std;
class NetThread {
public:
    NetThread();
    ~NetThread();
    int bind(string& ip, int& port);
    void run();
    void parseAddr(const string &addr_string, in_addr &addr_struct);
    void createEpoll();
    int accept(int fd);

private:
    Epoller epoller_;
    int shutdown_sock_fd_;
    int notify_sock_fd_;
    int sock_fd_;
    
};

#endif //EPOLLER_SERVER_H