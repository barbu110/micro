//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <event_loop.h>
#include <event_sources/net/await_connections.h>
#include <event_sources/net/receive.h>
#include <functional>
#include <map>
#include <netdb.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

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

    void close()
    {
      if (::close(fd) == -1)
      {
        throw microloop::KernelException(errno);
      }

      server->peer_connections.erase(fd);
    }
  };

  TcpServer(std::uint16_t port) : port{port}, server_fd{0}
  {
    auto port_str = std::to_string(port);

    addrinfo *results;
    addrinfo hints{.ai_socktype = SOCK_STREAM,
        .ai_family = AF_UNSPEC,
        .ai_flags = AI_PASSIVE | AI_NUMERICSERV};

    auto err_code = getaddrinfo(nullptr, port_str.c_str(), &hints, &results);
    if (err_code != 0)
    {
      std::stringstream err;
      err << __FUNCTION__ << ": " << gai_strerror(err_code);

      throw std::runtime_error(err.str());
    }

    auto r = results;
    for (; r != nullptr; r = r->ai_next)
    {
      server_fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
      if (server_fd == -1)
      {
        continue;
      }

      int val = 1;
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
      {
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

      if (bind(server_fd, r->ai_addr, r->ai_addrlen) == 0)
      {
        /*
         * We now have a valid socket.
         */
        break;
      }

      close(server_fd);
    }

    if (r == nullptr)
    {
      std::stringstream err;
      err << __FUNCTION__ << ": "
          << "Failed to bind to port " << port;

      throw std::runtime_error(err.str());
    }

    freeaddrinfo(results);

    if (listen(server_fd, 64) == -1)
    {
      throw microloop::KernelException(errno);
    }

    using namespace std::placeholders;
    auto connection_handler = std::bind(&TcpServer::handle_connection, this, _1, _2, _3);

    microloop::EventLoop::get_main()->add_event_source(
        new microloop::event_sources::net::AwaitConnections(server_fd, connection_handler));

    auto termination_handler = [&](std::uint32_t sig) {
      std::cout << "Terminating...\n";
      destroy();

      return true;
    };

    microloop::EventLoop::get_main()->register_signal_handler(SIGINT, termination_handler);
  }

  virtual ~TcpServer()  // TODO Do we need to send something to peer sockets?
  {
    destroy();
  }

private:
  void handle_connection(std::uint32_t fd, sockaddr_storage addr, socklen_t addrlen)
  {
    using microloop::event_sources::net::Receive;
    using namespace std::placeholders;

    peer_connections.emplace(fd, PeerConnection{this, addr, fd});

    auto on_recv = std::bind(&TcpServer::on_data, this, peer_connections[fd], _1);
    microloop::EventLoop::get_main()->add_event_source(new Receive<false>(fd, std::move(on_recv)));
  }

  void on_data(PeerConnection &conn, const microloop::Buffer &buf)
  {
    if (buf.empty())
    {
      /*
       * When the buffer is empty, the socket is in EOF condition. This indicates the cononection
       * has been closed by the remote peer, so the correct way to treat this condition on our side
       * is to close the socket.
       */

      conn.close();
      return;
    }

    std::cout << "Received data: length = " << buf.size() << "; " << static_cast<char *>(buf.data()) << "\n";
  }

  void destroy()
  {
    for (auto &it : peer_connections)
    {
      it.second.close();
    }

    close(server_fd);
  }

private:
  std::uint16_t port;
  std::uint32_t server_fd;

  std::map<std::uint32_t, PeerConnection> peer_connections;

  std::function<void(PeerConnection&)> connection_handler;
  std::function<void(PeerConnection&, const microloop::Buffer&)> data_handler;
};

}  // namespace microloop::net
