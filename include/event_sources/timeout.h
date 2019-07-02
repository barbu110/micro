// Copyright 2019 Victor Barbu.

#pragma once

#include <event_source.h>
#include <functional>
#include <errno.h>
#include <kernel_exception.h>
#include <sys/timerfd.h>
#include <unistd.h>

namespace microloop::event_sources {

class Timeout : public microloop::EventSource {
  using Types = TypeHelper<>;
public:

  Timeout(int timeout, Types::Callback callback) :
    microloop::EventSource{}, timeout{timeout}, callback{callback}
  {}

  virtual ~Timeout() override
  {
    int fd = get_id();
    close(fd);
  }

protected:
  virtual void start() override
  {
    errno = 0;
    int fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if (fd == -1) {
      throw microloop::KernelException(errno);
    }

    set_id(fd);

    itimerspec timer_value{{0, 0}, {timeout / 1000, timeout % 1000 * 1000000}};
    if (timerfd_settime(fd, 0, &timer_value, nullptr) == -1) {
      throw microloop::KernelException(errno);
    }
  }

  virtual void run_callback() override
  {
    callback();
  }

private:
  // Timeout in milliseconds.
  int timeout;

  // The callable object to call when the timer finishes.
  Types::Callback callback;
};

}  // namespace microloop::event_sources

#define MICROLOOP_SET_TIMEOUT(on_done, ms)                                                         \
  microloop::EventLoop::get_main()->add_event_source(                                              \
      new microloop::event_sources::Timeout(on_done, ms));
