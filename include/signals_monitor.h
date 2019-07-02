//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <functional>
#include <event_source.h>
#include <kernel_exception.h>
#include <signal.h>
#include <signals_monitor.h>
#include <sys/signalfd.h>
#include <unistd.h>

namespace microloop {

class SignalsMonitor : public EventSource {
public:
  using SignalHandler = std::function<bool(int signal)>;

  SignalsMonitor()
  {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1) {
      throw KernelException(errno);
    }

    int fd = signalfd(-1, &mask, SFD_NONBLOCK);
    if (fd == -1) {
      throw KernelException(errno);
    }

    set_id(fd);
  }

  ~SignalsMonitor()
  {
    int fd = get_id();
    close(fd);

    sigset_t mask;
    sigemptyset(&mask);

    sigprocmask(SIG_BLOCK, &mask, nullptr);
  }

private:
  int fd;
};

}  // namespace microloop
