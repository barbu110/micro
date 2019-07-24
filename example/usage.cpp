// Copyright 2019 Victor Barbu

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <iterator>
#include <microloop.h>
#include <net/tcp_server.h>
#include <netdb.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

using namespace microloop;

bool signal_handler(int sig)
{
  std::cout << "Received signal " << sig << "\n";
  return true;
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "usage: " << argv[0] << " <port>\n";
    return -1;
  }

  microloop::EventLoop::get_main()->register_signal_handler(SIGINT, signal_handler);

  auto port = std::stoi(argv[1]);
  auto tcp_server = microloop::net::TcpServer(port);

  while (true)
  {
    MICROLOOP_TICK();
  }
}
