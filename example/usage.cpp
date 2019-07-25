// Copyright 2019 Victor Barbu

#include <iostream>
#include <microloop.h>
#include <net/tcp_server.h>
#include <string>
#include <stdexcept>
#include <limits>

using namespace microloop;

void on_conn(net::TcpServer::PeerConnection &conn)
{
  std::cout << "Received connection\n";
}

void on_data(net::TcpServer::PeerConnection &conn, const microloop::Buffer &buf)
{
  if (buf.empty())
  {
    conn.close();
    return;
  }

  std::cout << "Received: " << static_cast<char *>(buf.data()) << "\n";
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "usage: " << argv[0] << " <port>\n";
    return -1;
  }

  auto port = std::stoi(argv[1]);
  if (port < 1 || port > std::numeric_limits<std::uint16_t>::max())
  {
    throw std::range_error("invalid port");
  }

  auto tcp_server = microloop::net::TcpServer{static_cast<uint16_t>(port), on_conn, on_data};

  while (true)
  {
    MICROLOOP_TICK();
  }
}
