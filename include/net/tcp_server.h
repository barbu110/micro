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
#include <signal.h>
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

  using ConnectionHandler = std::function<void(PeerConnection &)>;
  using DataHandler = std::function<void(PeerConnection &, const microloop::Buffer &)>;

public:
  TcpServer(std::uint16_t port);
  ~TcpServer();

  template <class Func, class... Args>
  void set_connection_callback(Func &&func, Args &&... args)
  {
    using namespace std::placeholders;

    auto bound = std::bind(std::forward<Func>(func), std::forward<Args>(args)..., _1);
    on_conn = std::move(bound);
  }

  template <class Func, class... Args>
  void set_data_callback(Func &&func, Args &&... args)
  {
    using namespace std::placeholders;

    auto bound = std::bind(std::forward<Func>(func), std::forward<Args>(args)..., _1, _2);
    on_data = std::move(bound);
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
