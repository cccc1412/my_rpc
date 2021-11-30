#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "Serialization.h"
#include "Type.h"

using namespace std;

int main(int argc, char *argv[]) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sockfd) {
    perror("Sock Created");
    exit(-1);
  }

  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  if (inet_aton("127.0.0.1", &server_addr.sin_addr) ==
      0)  //服务器的IP地址来自程序的参数
  {
    printf("Server IP Address Error!\n");
    exit(1);
  }

  server_addr.sin_port = htons(9996);
  socklen_t server_addr_length = sizeof(server_addr);
  //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
  if (connect(sockfd, (struct sockaddr *)&server_addr, server_addr_length) <
      0) {
    printf("Can Not Connect!\n");
    exit(1);
  }

  printf("Connect Successfull\n");

  vector<char> req;
  char res[1024] = {0};

  int a = 1;
  int b = (int)1e9;

  OutputStream *os = new OutputStream();
  String op = String("sum");

  InputStream *is = new InputStream();

  int loop = 100;
  int count = 0;
  for (int i = 0; i < loop; i++) {
    Float x = (rand() % (b - a)) + a;
    Float y = (rand() % (b - a)) + a;
    Float z = x + y;
    // printf("%f\n",z);
    os->reset();
    os->write(op, 0);
    os->write(x, 1);
    os->write(y, 2);
    req = os->getByteBuffer();

    write(sockfd, &req[0], req.size());
    read(sockfd, res, sizeof(res));

    is->reset();
    is->setBuffer(&res[0], sizeof(res));
    Float r;
    is->read(r, 1);
    // printf("%f\n",r);

    bool matched = abs(r - z) < 1e4;
    if (matched) {
      count++;
    }
    printf("Returned Value: %.4f, Expected Value: %.4f [%d]\n ", r, z, matched);

    memset(res, 0, sizeof(res));
  }
  printf("Accuracy %d out of %d\n", count, loop);

  close(sockfd);

  return 0;
}