#ifndef EPOLLER_H_
#define EPOLLER_H_
#include <stdint.h>
#include <sys/epoll.h>
class Epoller {
 public:
  Epoller(bool et = true);
  ~Epoller();
  void create(int max_connections);  // epoll服务支持的最大连接数
  void add(int fd, long long data, uint32_t event);
  void del(int fd, long long data, uint32_t event);
  void mod(int fd, long long data, uint32_t event);
  int wait(int milliseconds);
  epoll_event get(int i);

 protected:
  int epoll_fd_;
  int max_connections_;
  epoll_event *_pevs;  //事件集
  bool is_et_;
  void ctrl(int fd, long long data, uint32_t events, int op);
};

#endif  // EPOLLER_H_
