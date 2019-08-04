//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "signals_monitor.h"

#include "kernel_exception.h"

#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

namespace microloop
{

SignalsMonitor::SignalsMonitor() : EventSource{0}
{
  sigemptyset(&initial_sigset);
  if (sigprocmask(SIG_SETMASK, nullptr, &initial_sigset) == -1)
  {
    throw microloop::KernelException(errno);
  }

  int fd = signalfd(-1, &initial_sigset, SFD_NONBLOCK);
  if (fd == -1)
  {
    throw KernelException(errno);
  }

  set_fd(fd);
}

SignalsMonitor::~SignalsMonitor()
{
  close(get_fd());
  sigprocmask(SIG_BLOCK, &initial_sigset, nullptr);
}

void SignalsMonitor::run_callback()
{
  /*
   * When this callback is called, we expect the signals file descriptor to have data available
   * for read so the read syscall should not block.
   */

  signalfd_siginfo info{};
  ssize_t nread = read(get_fd(), &info, sizeof(signalfd_siginfo));
  if (nread == -1)
  {
    /*
     * TODO Check if a check for EAGAIN error is required here.
     */
    throw microloop::KernelException(errno);
  }

  bool can_exit = true;
  std::uint32_t signo = info.ssi_signo;
  for (const auto &fn : signal_handlers[signo])
  {
    if (!fn(signo))
    {
      can_exit = false;
    }
  }

  if (can_exit)
  {
    /*
     * TODO Find a way to have objects go out of scope and their destructors called before exiting.
     */
    _exit(signo);
  }
}

void SignalsMonitor::register_signal_handler(std::uint32_t sig, SignalHandler &&handler)
{
  update_sigmask(SIG_BLOCK, sig);

  signal_handlers[sig].push_back(handler);
}

void SignalsMonitor::update_sigmask(int how, int sig)
{
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, sig);

  if (sigprocmask(how, &mask, nullptr) == -1)
  {
    throw KernelException(errno);
  }

  sigset_t abs_mask;
  sigemptyset(&abs_mask);
  if (sigprocmask(SIG_SETMASK, nullptr, &abs_mask) == -1)
  {
    throw KernelException(errno);
  }

  int fd = signalfd(get_fd(), &abs_mask, SFD_NONBLOCK);  // TODO Check if fd is different.
  if (fd == -1)
  {
    throw microloop::KernelException(errno);
  }

  curr_sigset = abs_mask;
}

}  // namespace microloop
