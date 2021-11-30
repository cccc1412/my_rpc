#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>

#include "Serialization.h"
#include "Type.h"

using namespace std;
char dic[26];
void initdic() {
  for (int i = 0; i < 26; i++) {
    dic[i] = char(97 + i);
    // printf("%c\n",dic[i]);
  }
}

void randString(String &s) {
  int n = rand() % 50 + 1;
  for (int i = 0; i < n; i++) {
    s.push_back(dic[rand() % 26]);
  }
}

int main(int argc, char *argv[]) {
  initdic();
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
  String op = String("uppercase");

  InputStream *is = new InputStream();

  int loop = 100;
  int count = 0;
  for (int i = 0; i < loop; i++) {
    String s;
    randString(s);
    os->reset();
    os->write(op, 0);
    os->write(s, 1);
    req = os->getByteBuffer();

    transform(s.begin(), s.end(), s.begin(), ::toupper);

    write(sockfd, &req[0], req.size());
    read(sockfd, res, sizeof(res));

    is->reset();
    is->setBuffer(&res[0], sizeof(res));
    String r;
    is->read(r, 1);

    bool matched = s == r;
    if (matched) {
      count++;
    }
    printf("Returned Value: %s, Expected Value: %s [%d]\n ", r.c_str(),
           s.c_str(), matched);

    memset(res, 0, sizeof(res));
  }
  printf("Accuracy %d out of %d\n", count, loop);

  close(sockfd);

  return 0;
}