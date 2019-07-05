//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <buffer.h>
#include <event_source.h>
#include <unistd.h>
#include <sys/socket.h>
#include <kernel_exception.h>
#include <tuple>
#include <functional>

namespace microloop::event_sources::net {

class Send : public microloop::EventSource {
  using Types = microloop::TypeHelper<ssize_t>;

public:
  // The socket should be opened in nonblocking mode. We do not issue an fcntl to set
  // it that way because that would waste time.
  Send(int sock, const microloop::Buffer &buf, Types::Callback callback) :
      microloop::EventSource{sock},
      buf{buf}, callback{callback}
  {}

  void start() override
  {
    ssize_t sent = send(get_id(), buf.data(), buf.size(), 0);
    if (sent == -1) {
      throw microloop::KernelException(errno);
    }

    return_object = std::make_tuple(sent);
  }

  void run_callback() override
  {
    std::apply(callback, return_object);
  }

private:
  microloop::Buffer buf;
  Types::Callback callback;
  Types::ReturnType return_object;
};

}
