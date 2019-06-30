//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include <signals_monitor.h>

#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>

#include <kernel_exception.h>

namespace microloop {

SignalsMonitor::SignalsMonitor() : EventSource{}
{
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);

  if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1) {
    throw KernelException(errno);
  }

  fd = signalfd(-1, &mask, SFD_NONBLOCK);
  if (fd == -1) {
    throw KernelException(errno);
  }
}

SignalsMonitor::~SignalsMonitor()
{
  close(fd);
}

EventSource::TrackingData SignalsMonitor::get_tracking_data() const
{
  return {fd, -1};
}

}
