#ifndef EPOLLER_SERVER_H_
#define EPOLLER_SERVER_H

#include <string>
#include <iostream>
#include <netinet/in.h>
#include <map>
#include <vector>
#include <list>

#include "epoller.h"
#include "thread_queue.h"
#include "thread.h"

using namespace std;

struct RecvData {
    uint32_t uid;
    string buffer;
    string ip;
    uint16_t port;
    int64_t recv_time_stamp;
    bool is_over_load;
    bool is_close;
    int fd;
    int closeType;
};

struct SendData {
    char cmd;
    uint32_t uid;
    string buffer;
    string ip;
    uint16_t port;
};

struct RequestPacket {
    string func_name;
    vector<char>buffer;
};

struct ResponsePacket {
    vector<char>buffer;
};

class EpollerServer;

class NetThread {
public:
    NetThread();
    ~NetThread();
    int bind(string& ip, int& port);
    void run();
    void parseAddr(const string &addr_string, in_addr &addr_struct);
    void createEpoll();
    int accept(int fd);
    void send(uint32_t uid, const string &s, const string &ip, uint16_t port);
    void ProcessPipe();
    void ProcessNet(epoll_event &ev);

    void InsertRecvQueue(RecvData *recv_data);

    ThreadQueue<RecvData*, deque<RecvData*> > recv_buffer;
    ThreadQueue<SendData*, deque<SendData*> > send_buffer;

    bool WaitForRecvQueue(RecvData* &recv, uint32_t wait_time);
private:
    Epoller epoller_;
    int shutdown_sock_fd_;
    int notify_sock_fd_;
    int sock_fd_;

    map<int, int> listen_connect_fd; //uid->fd
    list<uint32_t> free_;//空闲的uid
    
};



class Handle:public Thread{
public:
    Handle();
    virtual ~Handle();

    void SetEpollServer(EpollerServer *p_epller_server);
    EpollerServer* GetEpollServer();
    virtual void Run();

public:
    void SendResponse(uint32_t uid, const string &s, const string &ip, int port);
    void close();
    virtual void initalize() {};
    bool WaitForRecvQueue(RecvData* &recv, uint32_t wait_time);

protected:
    EpollerServer* p_epoll_server_;
    uint32_t wait_time_;
    vector<Handle> handles_;

protected:
    virtual void HandleImp();
};

class EpollerServer {
public:
    EpollerServer();
    ~EpollerServer();

public:

    NetThread* GetNetThread();
    void Send(unsigned int uid, const string &s, const string &ip, uint16_t port);
    void SetNetThread(NetThread* net_thread) {net_threads_ = net_thread;};
private:
    NetThread* net_threads_;
};

#endif //EPOLLER_SERVER_H