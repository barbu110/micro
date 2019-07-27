//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "net/tcp_server.h"

#include "utils/error.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sstream>
#include <stdexcept>

namespace microloop::net
{

void TcpServer::PeerConnection::close()
{
  if (::close(fd) == -1)
  {
    throw microloop::KernelException(errno);
  }

  server->peer_connections.erase(fd);
}

std::uint32_t TcpServer::create_passive_socket(std::uint16_t port)
{
  auto port_str = std::to_string(port);

  addrinfo *results;
  addrinfo hints{
      .ai_socktype = SOCK_STREAM, .ai_family = AF_UNSPEC, .ai_flags = AI_PASSIVE | AI_NUMERICSERV};

  auto err_code = getaddrinfo(nullptr, port_str.c_str(), &hints, &results);
  if (err_code != 0)
  {
    std::stringstream err;
    err << __FUNCTION__ << ": " << gai_strerror(err_code);

    throw std::runtime_error(err.str());
  }

  std::uint32_t fd;
  auto r = results;
  for (; r != nullptr; r = r->ai_next)
  {
    fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
    if (fd == -1)
    {
      continue;
    }

    int val = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
    {
      std::stringstream err;
      err << __FUNCTION__ << ": " << microloop::utils::error::strerror(errno);

      throw std::runtime_error(err.str());
    }

    if (bind(fd, r->ai_addr, r->ai_addrlen) == 0)
    {
      /*
       * We now have a valid socket.
       */
      break;
    }

    close(fd);
  }

  if (r == nullptr)
  {
    std::stringstream err;
    err << __FUNCTION__ << ": "
        << "Failed to bind to port " << port;

    throw std::runtime_error(err.str());
  }

  freeaddrinfo(results);

  if (listen(fd, 64) == -1)
  {
    throw microloop::KernelException(errno);
  }

  return fd;
}

void TcpServer::handle_connection(std::uint32_t fd, sockaddr_storage addr, socklen_t addrlen)
{
  using microloop::event_sources::net::Receive;
  using namespace std::placeholders;

  PeerConnection conn{this, addr, fd};
  peer_connections.emplace(fd, conn);

  auto bound_on_data = std::bind(on_data, conn, _1);
  microloop::EventLoop::get_main()->add_event_source(new Receive<false>(fd, bound_on_data));

  on_conn(conn);
}

void TcpServer::destroy()
{
  for (auto &it : peer_connections)
  {
    it.second.close();
  }

  close(server_fd);
}

}  // namespace microloop::net
