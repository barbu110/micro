//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_source.h>
#include <cstdint>
#include <sys/socket.h>
#include <kernel_exception.h>
#include <errno.h>
#include <utility>

namespace microloop::event_sources::net {

class AwaitConnections : public microloop::EventSource {
  using Types = microloop::TypeHelper<std::uint32_t, sockaddr_storage, socklen_t>;

public:
  AwaitConnections(std::uint32_t sock, Types::Callback &&callback) :
      EventSource{sock},
      callback{std::move(callback)}
  {}

  void start() override
  {}

  /**
   * At this point, the server socket has data available, meaning that there is at least one
   * awaiting connection.
   */
  void run_callback() override
  {
    sockaddr_storage peer_addr;
    socklen_t addrlen = sizeof(sockaddr_storage);

    std::uint32_t conn_fd = accept(get_fd(), reinterpret_cast<sockaddr *>(&peer_addr), &addrlen);
    if (conn_fd == -1) {
      throw microloop::KernelException(errno);
    }

    return_object = std::make_tuple(conn_fd, peer_addr, addrlen);
    std::apply(callback, return_object);
  }

  virtual std::uint32_t produced_events() const override
  {
    return EPOLLIN;
  }

private:
  Types::ReturnType return_object;
  Types::Callback callback;
};

}
