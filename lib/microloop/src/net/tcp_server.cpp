//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/net/tcp_server.h"

#include "microloop/utils/error.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace microloop::net
{

void TcpServer::PeerConnection::close()
{
  EventLoop::instance().remove_event_source(event_source_);
  event_source_ = nullptr;

  if (::close(fd_) == -1)
  {
    throw microloop::KernelException(errno, __PRETTY_FUNCTION__);
  }
}

bool TcpServer::PeerConnection::send(const microloop::Buffer &buf)
{
  std::size_t total_sent = 0;

  while (total_sent != buf.size())
  {
    const std::uint8_t *data = static_cast<const std::uint8_t *>(buf.data());
    ssize_t nsent = ::send(fd_, data + total_sent, buf.size() - total_sent, 0);
    if (nsent == -1)
    {
      /*
       * TODO Process the error correctly.
       */

      return false;
    }

    total_sent += nsent;
  }

  return true;
}

bool TcpServer::PeerConnection::send_file(const std::filesystem::path &path)
{
  auto read_fd = open(path.c_str(), O_RDONLY);
  if (read_fd == -1)
  {
    return false;
  }

  struct stat stat_buf
  {};
  off_t offset = 0;

  fstat(read_fd, &stat_buf);

  sendfile(fd_, read_fd, &offset, stat_buf.st_size);
  ::close(read_fd);

  return true;
}

std::string TcpServer::PeerConnection::str(bool include_fd) const
{
  static constexpr std::size_t port_strlen = 8;

  char hostbuf[INET6_ADDRSTRLEN]{};
  char portbuf[port_strlen]{};

  auto err_code = getnameinfo(reinterpret_cast<const sockaddr *>(&addr_), addrlen_, hostbuf,
      sizeof(hostbuf), portbuf, sizeof(portbuf), NI_NUMERICHOST | NI_NUMERICSERV);
  if (err_code != 0)
  {
    std::stringstream err;
    err << __PRETTY_FUNCTION__ << ": " << gai_strerror(err_code);

    throw std::runtime_error(err.str());
  }

  std::stringstream address;
  address << hostbuf << ":" << portbuf;

  if (include_fd)
  {
    address << " - " << fd_;
  }

  return address.str();
}

TcpServer::TcpServer(std::uint16_t port) : port{port}
{
  using namespace std::placeholders;
  using microloop::EventLoop;
  using microloop::event_sources::net::AwaitConnections;

  auto server_fd = create_passive_socket(port);

  auto connection_handler = std::bind(&TcpServer::handle_connection, this, _1, _2, _3);
  EventLoop::instance().add_event_source(new AwaitConnections(server_fd, connection_handler));

  microloop::EventLoop::instance().register_signal_handler(SIGINT, [](std::uint32_t) {
    /*
     * This is here just to allow the application to exit smoothly, performing stack unwinding and
     * every other avaiable clean up.
     */
    return true;
  });

  fd_ = server_fd;
}

std::uint32_t TcpServer::create_passive_socket(std::uint16_t port)
{
  auto port_str = std::to_string(port);

  addrinfo *results;
  addrinfo hints{
      .ai_flags = AI_PASSIVE | AI_NUMERICSERV, .ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};

  auto err_code = getaddrinfo(nullptr, port_str.c_str(), &hints, &results);
  if (err_code != 0)
  {
    std::stringstream err;
    err << __PRETTY_FUNCTION__ << ": " << gai_strerror(err_code);

    throw std::runtime_error(err.str());
  }

  std::int32_t fd;
  auto r = results;
  for (; r != nullptr; r = r->ai_next)
  {
    fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
    if (fd < 0)
    {
      continue;
    }

    int val = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
    {
      std::stringstream err;
      err << __PRETTY_FUNCTION__ << ": " << microloop::utils::error::strerror(errno);

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
    err << __PRETTY_FUNCTION__ << ": "
        << "Failed to bind to port " << port;

    throw std::runtime_error(err.str());
  }

  freeaddrinfo(results);

  if (listen(fd, 64) == -1)
  {
    throw microloop::KernelException(errno);
  }

  return static_cast<std::uint32_t>(fd);
}

void TcpServer::handle_connection(std::uint32_t fd, sockaddr_storage addr, socklen_t addrlen)
{
  using microloop::EventLoop;
  using microloop::event_sources::net::Receive;
  using namespace std::placeholders;

  auto [it, inserted] = peer_connections.try_emplace(fd, this, addr, addrlen, fd);
  if (!inserted)
  {
    throw std::runtime_error("a connection with the same file descriptor already exists");
  }

  auto &peer_conn = it->second;
  auto event_source = new Receive<false>(fd);

  peer_conn.event_source_ = event_source;
  event_source->set_on_recv(std::bind(on_data, std::ref(peer_conn), _1));

  EventLoop::instance().add_event_source(event_source);

  on_conn(peer_conn);
}

void TcpServer::close_conn(TcpServer::PeerConnection &conn)
{
  peer_connections.erase(conn.fd());
}

}  // namespace microloop::net
