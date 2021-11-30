#include <iostream>
#include <memory>

#include "Servant.h"
#include "SumImp.h"
#include "UppercaseImp.h"
#include "epoller.h"
#include "epoller_server.h"

int main() {
  int handle_num = 3;
  EpollerServer *epoll_server = new EpollerServer();
  NetThread *net_thread = new NetThread();
  epoll_server->SetNetThread(net_thread);
  string ip = "127.0.0.1";
  int port = 9996;
  net_thread->bind(ip, port);
  net_thread->createEpoll();
  vector<Handle *> handles;
  for (int i = 0; i < handle_num; i++) {
    Handle *handle = new Handle();
    handle->SetEpollServer(epoll_server);
    handle->Start();
    handles.push_back(handle);
  }

  // for(int i = 0; i< handle_num; i++) {//为什么不对？
  //     Handle handle;
  //     handle.SetEpollServer(epoll_server);
  //     handle.Start();
  //     handles.push_back(&handle);
  // }
  // for(auto& handle:handles) {
  //     handle->Start();
  // }
  net_thread->run();
  return 0;
}