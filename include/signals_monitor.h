//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_source.h>
#include <functional>
#include <kernel_exception.h>
#include <signal.h>
#include <signals_monitor.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <cstdint>
#include <map>
#include <vector>
#include <unistd.h>

namespace microloop {

class SignalsMonitor : public EventSource {
public:
  using SignalHandler = std::function<void(std::uint32_t signal)>;

  SignalsMonitor() : EventSource{0}
  {
    sigemptyset(&initial_sigset);
    if (sigprocmask(SIG_SETMASK, nullptr, &initial_sigset) == -1) {
      throw microloop::KernelException(errno);
    }

    int fd = signalfd(-1, &initial_sigset, SFD_NONBLOCK);
    if (fd == -1) {
      throw KernelException(errno);
    }

    set_fd(fd);
  }

  ~SignalsMonitor()
  {
    close(get_fd());
    sigprocmask(SIG_BLOCK, &initial_sigset, nullptr);
  }

  void start() override
  {}

  void run_callback() override  // TODO Check if we still need to make sure EAGAIN is not returned.
  {
    /*
     * When this callback is called, we expect the signals file descriptor to have data available
     * for read so the read syscall should not block.
     */

     signalfd_siginfo info{};
     ssize_t nread = read(get_fd(), &info, sizeof(signalfd_siginfo));
     if (nread == -1) {
       throw microloop::KernelException(errno);
     }

     std::uint32_t signo = info.ssi_signo;
     for (const auto &fn : signal_handlers[signo]) {
       fn(signo);
     }
  }

  virtual std::uint32_t produced_events() const override
  {
    return EPOLLIN;
  }

  void register_signal_handler(std::uint32_t sig, SignalHandler &&handler)
  {
    update_sigmask(SIG_BLOCK, sig);

    signal_handlers[sig].push_back(handler);
  }

  const sigset_t &get_sigmask() const
  {
    return curr_sigset;
  }

private:
  void update_sigmask(int how, int sig)
  {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);

    if (sigprocmask(how, &mask, nullptr) == -1) {
      throw KernelException(errno);
    }

    sigset_t abs_mask;
    sigemptyset(&abs_mask);
    if (sigprocmask(SIG_SETMASK, nullptr, &abs_mask) == -1) {
      throw KernelException(errno);
    }

    int fd = signalfd(get_fd(), &abs_mask, SFD_NONBLOCK);  // TODO Check if fd is different.
    if (fd == -1) {
      throw microloop::KernelException(errno);
    }

    curr_sigset = abs_mask;
  }

  sigset_t initial_sigset;
  sigset_t curr_sigset;
  std::map<std::uint32_t, std::vector<std::function<void(std::uint32_t)>>> signal_handlers;

};

}  // namespace microloop
