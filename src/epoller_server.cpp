#include "epoller_server.h"
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <errno.h>

using namespace std;

EpollerServer::EpollerServer() {
    net_threads_ = new NetThread();
}

EpollerServer::~EpollerServer() {
    delete net_threads_;
}

void EpollerServer::Send(unsigned int uid, const string &s, const string &ip, uint16_t port) {
    net_threads_->send(uid, s, ip, port);
}



NetThread::NetThread() {
    // shutdown_sock_fd_ = socket(AF_INET, SOCK_STREAM,0);
    // notify_sock_fd_ = socket(AF_INET, SOCK_STREAM,0);
    // if(shutdown_sock_fd_ < 0) {
    //     cout<<"create shunt down socket error!"<<endl;
    // }
    // if(notify_sock_fd_ < 0) {
    //     cout<<"create notify socket error!"<<endl;
    // }
}

NetThread::~NetThread() {

}

void NetThread::parseAddr(const string &addr_string, in_addr &addr_struct) {
    int ret = inet_pton(AF_INET, addr_string.c_str(), &addr_struct);//点分十进制转化为二进制整数
    if(ret<0) {
        cout<<"parseAddr error!"<<endl;
    } else {
        //??不知道干嘛
    }
}

int NetThread::bind(string& ip, int& port) {
    //创建socket, bind, listen 
    sock_fd_ = socket(AF_INET, SOCK_STREAM,0);
    if(sock_fd_ < 0) {
        cout<<"create bind socket error!"<<endl;
    }
    sockaddr_in bind_addr;
    memset(&bind_addr,0,sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);//转化网路字节序16位
    parseAddr(ip, bind_addr.sin_addr);

    int ret = ::bind(sock_fd_, (sockaddr*)&bind_addr, sizeof(bind_addr));//::是全局作用域符
    if(ret < 0) {
        cout<<"bind error!"<<endl;
        cout<<"errorno is: "<<errno<<endl;
    }else {
        cout<<"server bind fd:"<<sock_fd_<<endl;
    }
    ret = ::listen(sock_fd_, 1024);
    if(ret < 0 ) {
        cout<<"listen error!"<<endl;
    }
    return sock_fd_;
}

int NetThread::accept(int fd) {
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(sock_fd_, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        cout<<"accept error! errno is: "<<errno<<endl;
    }else {
        cout<<"accept new fd: "<<client_fd<<endl;
    }
    return client_fd;
}

void NetThread::run() {
    while(1) {
        int ev_num = epoller_.wait(2000);
        for(int i = 0; i < ev_num; i++) {
            epoll_event e = epoller_.get(i);
            //std::cout<<e.data.fd<<std::endl;
            if(e.data.fd == sock_fd_) {//新的连接
                int client_fd = accept(sock_fd_);
                if(client_fd <= 0) continue;
                epoll_event ev;
                ev.data.fd = client_fd;
                epoller_.add(client_fd, NULL, EPOLLIN | EPOLLET);
            }else if(e.data.fd == notify_sock_fd_) {//发送数据
                ProcessPipe();
            }else { //接受数据
                ProcessNet(e);
            }
        }
    }
}

void NetThread::createEpoll() {//创建epoll，对三个文件描述符添加事件监听
    epoller_.create(1024);
    epoller_.add(sock_fd_, NULL, EPOLLIN);//EPOLLIN 表示有新数据到达，socket可读
    //epoller_.add(shutdown_sock_fd_, NULL, EPOLLIN);
    epoller_.add(notify_sock_fd_, NULL, EPOLLIN);
}

void NetThread::send(uint32_t uid, const string &s, const string &ip, uint16_t port) {
    SendData* send_data = new SendData();
    send_data->uid = uid;
    send_data->cmd = 's';
    send_data->buffer = s;
    send_data->ip = ip;
    send_data->port = port;
    send_buffer.push_back(send_data);
    epoller_.mod(notify_sock_fd_, NULL, EPOLLOUT);
}

void NetThread::InsertRecvQueue(RecvData *recv_data) {
    recv_buffer.push_back(recv_data);
    //monitor.notify();
}

void NetThread::ProcessPipe() {
    deque<SendData*> send_data;
    send_buffer.swap(send_data);
    for(auto it = send_data.begin(); it != send_data.end(); it++) {
        if('s' == (*it)->cmd) {
            uint32_t uid = (*it)->uid;
            //int fd = listen_connect_fd[uid];
            int bytes = ::send(uid, (*it)->buffer.c_str(), (*it)->buffer.size(), 0);
            cout<<"send bytes :"<<bytes<<endl;
        }
    }
}

void NetThread::ProcessNet(epoll_event &ev) {
    int client_fd = ev.data.fd;
    char buffer[1024] = {0};
    int ret = recv(client_fd, buffer, 1024, 0);
    if(ret < 0) {
        cout<<"recev error! errno is: "<<errno<<endl;
        return;
    }else if(ret == 0) {
        cout<<"client close connect!"<<endl;
        return;
    }else {
        cout<<"receive "<<ret<<" bytes: "<<buffer<<endl;
    } 
    RecvData *recv = new RecvData();
    recv->buffer = recv->buffer.append(buffer, ret);
    recv->fd = client_fd;
    InsertRecvQueue(recv);
    //send();
}

bool NetThread::WaitForRecvQueue(RecvData* &recv, uint32_t wait_time) {
    bool ret = false;
    ret = recv_buffer.pop_front(recv, wait_time);
    return ret;
}

Handle::Handle() {

}

Handle::~Handle() {

}

void Handle::SendResponse(uint32_t uid, const string &s, const string &ip, int port) {
    p_epoll_server_->Send(uid, s, ip, port);
}

void Handle::SetEpollServer(EpollerServer *p_epoll_server) {
    p_epoll_server_ = p_epoll_server;
}

NetThread* EpollerServer::GetNetThread() {
    return net_threads_;
}
void Handle::Run() {
    initalize();
    HandleImp();
}

void Handle::HandleImp() {
    cout<<"Handle::handleImp"<<endl;
    RecvData* recv = NULL;
    while(true) {
        NetThread* netThread = p_epoll_server_->GetNetThread();
        //netThread->monitor.timedWait(100);
        while(WaitForRecvQueue(recv, 0)) {
            cout<<"handleImp recv uid  is "<<recv->fd<<endl;
            //compute
            p_epoll_server_->Send(recv->fd,recv->buffer, "0", 0);
        }
    }
}

bool Handle::WaitForRecvQueue(RecvData* &recv, uint32_t wait_time) {
    NetThread* net_thread = p_epoll_server_->GetNetThread();
    return net_thread->WaitForRecvQueue(recv, wait_time);
}