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
#include <event_sources/net/listen.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>

using namespace microloop;

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <port>\n";
    return -1;
  }

  int port = std::stoi(argv[1]);

  sockaddr_in svaddr;
  socklen_t len;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "failed to open socket\n";
  }

  if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
    std::cerr << "fcntl\n";
  }

  addrinfo *result;
  int optval;

  addrinfo hints{};
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

  std::cout << "Port: " << port << "\n";

  if (getaddrinfo(nullptr, (const char *) port, &hints, &result) != 0) {
    perror("getaddrinfo");
    return 1;
  }

  std::uint32_t serverfd;
  for (auto p = result; p != nullptr; p = p->ai_next) {
    serverfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (serverfd == -1) {
      continue;
    }

    int val = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
      perror("setsockopt");
      return 1;
    }

    if (bind(serverfd, p->ai_addr, p->ai_addrlen) == 0) {
      break;  // We have a valid server socket.
    }

    close(serverfd);
  }

  freeaddrinfo(result);

  microloop::EventLoop::get_main()->add_event_source(
    new microloop::event_sources::net::Listen(serverfd, port)
  );

  // while (true) {
  //   MICROLOOP_TICK();
  // }
}
