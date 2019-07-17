// Copyright 2019 Victor Barbu

#include <iostream>
#include <microloop.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <arpa/inet.h>
#include <algorithm>
#include <sstream>
#include <iterator>

using namespace microloop;

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <ip>:<port>\n";
    return -1;
  }

  std::stringstream addr{argv[1]};
  std::string ip, port_str;
  std::getline(addr, ip, ':');
  std::getline(addr, port_str, ':');

  int port = std::stoi(port_str);

  sockaddr_in svaddr;
  socklen_t len;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "failed to open socket\n";
  }

  svaddr.sin_family = AF_INET;
  svaddr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip.c_str(), &svaddr.sin_addr) < 0) {
    std::cerr << "pton failed\n";
  }

  if (connect(sockfd, reinterpret_cast<sockaddr*>(&svaddr), sizeof(svaddr)) < 0) {
    std::cerr << "connect failed\n";
  }

  if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
    std::cerr << "fcntl\n";
  }

  while (true) {
    MICROLOOP_TICK();
  }
}
