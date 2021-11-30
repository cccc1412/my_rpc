#include "epoller.h"

#include <unistd.h>

void Epoller::create(int max_connections) {
  max_connections_ = max_connections;
  epoll_fd_ = epoll_create(max_connections_ + 1);
  delete[] _pevs;
  _pevs = new epoll_event[max_connections_ + 1];
}

void Epoller::ctrl(int fd, long long data, uint32_t events, int op) {
  epoll_event ev;
  ev.data.u64 = data;
  if (is_et_) {
    ev.events = events | EPOLLET;
  } else {
    ev.events = events;
  }
  // ev.data.fd = fd;
  epoll_ctl(epoll_fd_, op, fd, &ev);
}

void Epoller::add(int fd, long long data, uint32_t event) {
  ctrl(fd, data, event, EPOLL_CTL_ADD);
}

void Epoller::mod(int fd, long long data, uint32_t event) {
  ctrl(fd, data, event, EPOLL_CTL_MOD);
}

void Epoller::del(int fd, long long data, uint32_t event) {
  ctrl(fd, data, event, EPOLL_CTL_DEL);
}

int Epoller::wait(int milliseconds) {
  return epoll_wait(epoll_fd_, _pevs, max_connections_ + 1, milliseconds);
}

epoll_event Epoller::get(int i) { return _pevs[i]; }
Epoller::Epoller(bool et) {}

Epoller::~Epoller() {}
