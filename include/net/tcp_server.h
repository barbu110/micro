//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <string>
#include <sstream>
#include <cstdint>
#include <cstring>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <event_loop.h>
#include <event_sources/net/await_connections.h>
#include <errno.h>
#include <functional>
#include <map>

namespace microloop::net {

class TcpServer {
public:
  struct PeerConnection {
    TcpServer *server;
    sockaddr_storage addr;
    std::uint32_t fd;
  };

  TcpServer(std::uint16_t port) : port{port}, server_fd{0}
  {
    auto port_str = std::to_string(port);

    addrinfo *results;
    addrinfo hints{
      .ai_socktype = SOCK_STREAM,
      .ai_family = AF_UNSPEC,
      .ai_flags = AI_PASSIVE | AI_NUMERICSERV
    };

    auto err_code = getaddrinfo(nullptr, port_str.c_str(), &hints, &results);
    if (err_code != 0) {
      std::stringstream err;
      err << __FUNCTION__ << ": " << gai_strerror(err_code);

      throw std::runtime_error(err.str());
    }

    auto r = results;
    for (; r != nullptr; r = r->ai_next) {
      server_fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
      if (server_fd == -1) {
        continue;
      }

      int val = 1;
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
        /*
         * See http://man7.org/linux/man-pages/man3/strerror.3.html#NOTES for explanation
         */
        static const std::size_t safe_err_len = 1024;
        char buf[safe_err_len] = {};
        strerror_r(errno, buf, safe_err_len);

        std::stringstream err;
        err << __FUNCTION__ << ": " << buf;

        throw std::runtime_error(err.str());
      }

      if (bind(server_fd, r->ai_addr, r->ai_addrlen) == 0) {
        /*
         * We now have a valid socket.
         */
        break;
      }

      close(server_fd);
    }

    if (server_fd == 0) {
      std::stringstream err;
      err << __FUNCTION__ << ": " << "Failed to bind to port " << port;

      throw std::runtime_error(err.str());
    }

    freeaddrinfo(results);

    if (listen(server_fd, 64) == -1) {
      throw microloop::KernelException(errno);
    }

    using namespace std::placeholders;
    auto connection_handler = std::bind(&TcpServer::handle_connection, this, _1, _2, _3);

    microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::net::AwaitConnections(server_fd, connection_handler)
    );

    while (true) {
      MICROLOOP_TICK();
    }
  }

  virtual ~TcpServer()  // TODO Do we need to send something to peer sockets?
  {
    close(server_fd);
  }

  // template <class Func>
  // virtual void on_connect(Func &&fn);
  //
  // template <class Func>
  // virtual void on_request(Func &&fn);
  //
  // template <class Func>
  // virtual void on_disconnect(Func &&fn);
  //
  // template <class Func>
  // virtual void on_shutdown(Func &&fn);

private:
  void handle_connection(std::uint32_t fd, sockaddr_storage addr, socklen_t addrlen)
  {
    std::cout << "Connection received on socket " << fd << "\n";

    peer_connections.emplace(fd, PeerConnection{this, addr, fd});
  }

private:
  std::uint16_t port;
  std::uint32_t server_fd;

  std::map<std::uint32_t, PeerConnection> peer_connections;
};

}
