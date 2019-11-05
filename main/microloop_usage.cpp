//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/net/tcp_server.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <sstream>

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

  std::cout << "Received: " << buf.str_view() << "\n";

  std::ostringstream ss{"You wrote: "};
  ss << buf.str_view() << std::endl << "Goodbye\n";

  conn.send(microloop::Buffer(ss.str().c_str()));
  conn.close();
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

  auto tcp_server = microloop::net::TcpServer{static_cast<uint16_t>(port)};
  tcp_server.set_connection_callback(on_conn);
  tcp_server.set_data_callback(on_data);

  while (true)
  {
    MICROLOOP_TICK();
  }
}
