// Copyright 2019 Victor Barbu

#include <iostream>
#include <microloop.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <arpa/inet.h>

using namespace microloop;

int main()
{
  sockaddr_in svaddr;
  socklen_t len;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "failed to open socket\n";
  }

  svaddr.sin_family = AF_INET;
  svaddr.sin_port = htons(5000);

  if (inet_pton(AF_INET, "127.0.0.1", &svaddr.sin_addr) < 0) {
    std::cerr << "pton failed\n";
  }

  if (connect(sockfd, reinterpret_cast<sockaddr*>(&svaddr), sizeof(svaddr)) < 0) {
    std::cerr << "connect failed\n";
  }

  fs::write(sockfd, "victor", [](ssize_t written) {
    std::cout << "Written " << written << " bytes.\n";
  });
  //
  // timers::set_timeout(2000, []() {
  //   std::cout << "Timeout 1 is done. 2000ms passed.\n";
  // });
  //
  // timers::set_timeout(400, []() {
  //   std::cout << "Timeout 2 is done. 400ms passed.\n";
  // });

  while (true) {
    MICROLOOP_TICK();
  }
}
