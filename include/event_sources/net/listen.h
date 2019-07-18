//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_source.h>
#include <cstdint>
#include <sys/socket.h>
#include <kernel_exception.h>
#include <iostream>

namespace microloop::event_sources::net {

class Listen : public microloop::EventSource {
  using Types = microloop::TypeHelper<>;

public:
  Listen(std::uint32_t sock, std::uint16_t port) : EventSource{sock}, port{port}
  {}

  void start() override
  {
    if (listen(get_fd(), 0) == -1) {
      throw microloop::KernelException(errno);
    }
  }

  void run_callback() override
  {
    std::cout << "incoming connection, apparently...\n";
  }
private:
  std::uint16_t port;

};

}
