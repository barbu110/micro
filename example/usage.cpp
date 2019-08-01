// Copyright 2019 Victor Barbu

#include "microloop.h"
#include "net/tcp_server.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

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

  conn.send("goodbye.\n");
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

  auto event_loop = microloop::EventLoop::get_main();

  auto tcp_server = microloop::net::TcpServer{static_cast<uint16_t>(port)};
  tcp_server.set_connection_callback(on_conn);
  tcp_server.set_data_callback(on_data);

  using microloop::event_sources::TimerController;

  microloop::timers::set_timeout(3000, event_loop, [](TimerController &) {
    std::cout << "Timer is done.\n";
  });

  std::uint32_t expirations_count = 0;
  microloop::timers::set_interval(2000, event_loop, [&expirations_count](TimerController &timer) {
    expirations_count++;
    std::cout << "Interval is done.\n";

    if (expirations_count == 4)
    {
      timer.cancel();
    }
  });

  while (true)
  {
    MICROLOOP_TICK();
  }
}
