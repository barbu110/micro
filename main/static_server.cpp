//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/request_parser.h"
#include "microloop/net/tcp_server.h"

#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace microloop;

struct BasicHttpServer
{
  microloop::net::TcpServer server_;
  std::map<std::int32_t, microhttp::http::RequestParser> clients_;

  BasicHttpServer(int port) : server_{port}
  {
    server_.set_connection_callback(&BasicHttpServer::on_conn, this);
    server_.set_data_callback(&BasicHttpServer::on_data, this);
  }

  void on_conn(net::TcpServer::PeerConnection &conn)
  {
    if (clients_.find(conn.fd) == clients_.end())
    {
      clients_[conn.fd] = microhttp::http::RequestParser();
    }
  }

  void on_data(net::TcpServer::PeerConnection &conn, const microloop::Buffer &buf)
  {
    if (buf.empty())
    {
      conn.close();
      return;
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    auto &parser = clients_[conn.fd];
    parser.add_chunk(buf);

    conn.send(
        "HTTP/1.1 200 OK\r\nServer: microhttp/0.9\r\nContent-Length: "
        "167\r\n\r\n<html><head><title>Micro Server</title></head><body><h1>Micro HTTP</h1><p>This "
        "is the <code>microhttp</code> framework powered by <code>microloop</code>.</body></html>");

    const auto &request = parser.get_parsed_request();

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dur_ms{t2 - t1};

    std::cout << "[" << conn.str() << "] " << request.get_http_method() << " " << request.get_uri()
              << "- " << dur_ms.count() << "ms\n";

    clients_[conn.fd].reset();
  }

  void run()
  {
    while (true)
    {
      MICROLOOP_TICK();
    }
  }
};

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

  BasicHttpServer server{port};
  server.run();
}
