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
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <net/tcp_server.h>
#include <signal.h>

using namespace microloop;

void signal_handler(int sig)
{
  std::cout << "Received signal " << sig << "\n";
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <port>\n";
    return -1;
  }

  microloop::EventLoop::get_main()->register_signal_handler(SIGINT, signal_handler);

  auto port = std::stoi(argv[1]);
  auto tcp_server = microloop::net::TcpServer(port);
}
