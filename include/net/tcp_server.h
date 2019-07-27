//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <event_loop.h>
#include <event_sources/net/await_connections.h>
#include <event_sources/net/receive.h>
#include <functional>
#include <map>
#include <string>
#include <sys/socket.h>

namespace microloop::net
{

class TcpServer
{
public:
  struct PeerConnection
  {
    TcpServer *server;
    sockaddr_storage addr;
    std::uint32_t fd;

    /**
     * Close this connection.
     */
    void close();
  };

private:
  using ConnectionHandler = std::function<void(PeerConnection &)>;
  using DataHandler = std::function<void(PeerConnection &, const microloop::Buffer &)>;

public:
  TcpServer(std::uint16_t port, ConnectionHandler &&on_conn, DataHandler &&on_data) :
      port{port}, server_fd{0}, on_conn{std::move(on_conn)}, on_data{std::move(on_data)}
  {
    using namespace std::placeholders;
    using microloop::EventLoop;
    using microloop::event_sources::net::AwaitConnections;

    server_fd = create_passive_socket(port);

    auto connection_handler = std::bind(&TcpServer::handle_connection, this, _1, _2, _3);
    EventLoop::get_main()->add_event_source(new AwaitConnections(server_fd, connection_handler));

    microloop::EventLoop::get_main()->register_signal_handler(SIGINT, [&](std::uint32_t) {
      destroy();
      return true;
    });
  }

  virtual ~TcpServer()
  {
    destroy();
  }

private:
  /**
   * Create a passive socket listening on an unspecified address on either IPv4 or IPv6 on the
   * given port.
   * @param  port The port to listen on.
   * @return A non-negative file descriptor of the TCP passive socket.
   */
  static std::uint32_t create_passive_socket(std::uint16_t port);

  /**
   * Internally handles an incoming connection on the TCP passive socket.
   * @param fd      The file descriptor of the accepted connection.
   * @param addr    Address representation for the accepted connection.
   * @param addrlen The size of the address representation structure.
   */
  void handle_connection(std::uint32_t fd, sockaddr_storage addr, socklen_t addrlen);

  /**
   * Destroys the TCP passive socket and all the active connections.
   */
  void destroy();

private:
  std::uint16_t port;
  std::uint32_t server_fd;

  std::map<std::uint32_t, PeerConnection> peer_connections;

  ConnectionHandler on_conn;
  DataHandler on_data;
};

}  // namespace microloop::net
