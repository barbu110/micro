//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microloop/event_loop.h"
#include "microloop/event_sources/net/await_connections.h"
#include "microloop/event_sources/net/receive.h"

#include <cstdint>
#include <filesystem>
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
  class PeerConnection
  {
  public:
    PeerConnection(TcpServer *server, sockaddr_storage addr, socklen_t addrlen, std::uint32_t fd) :
        server_{server}, addr_{addr}, addrlen_{addrlen}, fd_{fd}
    {}

    PeerConnection(const PeerConnection &) = delete;

    ~PeerConnection()
    {
      close();
    }

    /**
     * Send a buffer to the peer socket of this connection.
     * @param  buf The buffer to be sent to the peer socket.
     * @return Whether the operation succeeded or not. If only a fraction of the entire buffer is
     * sent, then the functino will report it as a failure.
     */
    bool send(const microloop::Buffer &);

    /**
     * Send the file identified by \p path parameter to the peer socket of this connection.
     * \param path The path in a reachable file system for the file to be sent.
     * \return Whether the operaiton succeeded or not.
     */
    bool send_file(const std::filesystem::path &path);

    /**
     * \brief Get a string representation of this peer connection. The representation will contain
     * a pretty representation of the socket address.
     * \param include_fd Whether to include the file descriptor in the string representation. Having
     * this set to `true` will effectively append the " - {fd}" format to the representation.
     * \return The string representation of a peer connection with the following format:
     *     <ip>:<port> [ - <fd>]
     */
    std::string str(bool include_fd = true) const;

    std::uint32_t fd() const
    {
      return fd_;
    }

    auto addr() const
    {
      return std::make_pair(addr_, addrlen_);
    }

  private:
    /**
     * \brief Close this connection.
     */
    void close();

  private:
    friend class TcpServer;

    TcpServer *server_;
    microloop::EventSource *event_source_ = nullptr;
    sockaddr_storage addr_;
    socklen_t addrlen_;
    std::uint32_t fd_;
  };

  using ConnectionHandler = std::function<void(PeerConnection &)>;
  using DataHandler = std::function<void(PeerConnection &, const microloop::Buffer &)>;

public:
  TcpServer(std::uint16_t port);

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

  /**
   * \brief Close the given connection.
   *
   * This will invalidate any existing reference to that connection. Users of this API must make
   * sure to consider this connection destroyed from this point on.
   */
  void close_conn(PeerConnection &conn);

  /**
   * \brief Get the file descriptor of the TCP server.
   *
   * This can be used to get low-level access to the server in order to retrieve or change socket
   * parameters.
   */
  std::uint32_t fd() const
  {
    return fd_;
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

private:
  std::uint16_t port;
  std::uint32_t fd_;

  std::map<std::uint32_t, PeerConnection> peer_connections;

  ConnectionHandler on_conn;
  DataHandler on_data;
};

}  // namespace microloop::net
