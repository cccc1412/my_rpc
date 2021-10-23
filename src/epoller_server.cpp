#include "epoller_server.h"
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <errno.h>

using namespace std;
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
            if(e.data.fd == sock_fd_) {//新的连接
                int client_fd = accept(sock_fd_);
                if(client_fd <= 0) continue;
                epoll_event ev;
                ev.data.fd = client_fd;
                epoller_.add(client_fd, NULL, EPOLLIN | EPOLLET);
            }else {
                int client_fd = e.data.fd;
                char buffer[1024] = {0};
                int ret = recv(client_fd, buffer, 1024, 0);
                if(ret < 0) {
                    cout<<"recev error! errno is: "<<errno<<endl;
                }else if(ret == 0) {
                    cout<<"client close connect!"<<endl;
                }else {
                    cout<<"receive "<<ret<<" bytes: "<<buffer<<endl;
                } 
            }
        }
    }
}

void NetThread::createEpoll() {//创建epoll，对三个文件描述符添加事件监听
    epoller_.create(1024);
    epoller_.add(sock_fd_, NULL, EPOLLIN);//EPOLLIN 表示有新数据到达，socket可读
    //epoller_.add(shutdown_sock_fd_, NULL, EPOLLIN);
    //epoller_.add(notify_sock_fd_, NULL, EPOLLIN);
}