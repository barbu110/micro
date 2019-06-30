// Copyright 2019 Victor Barbu

#include <errno.h>
#include <event_sources/timeout.h>
#include <kernel_exception.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

namespace microloop::event_sources {

Timeout::Timeout(Timeout::callback_type callback, int timeout) :
    microloop::EventSource {}, timeout { timeout }, callback { callback }
{}

Timeout::~Timeout()
{
  close(fd);
}

microloop::EventSource::TrackingData Timeout::get_tracking_data() const
{
  return { fd, -1 };
}

void Timeout::start()
{
  errno = 0;
  fd = timerfd_create(CLOCK_REALTIME, 0);
  if (fd == -1) {
    throw microloop::KernelException(errno);
  }

  itimerspec timer_value { { 0, 0 }, { timeout / 1000, timeout % 1000 * 1000000 } };
  if (timerfd_settime(fd, 0, &timer_value, nullptr) == -1) {
    throw microloop::KernelException(errno);
  }
}

void Timeout::cleanup()
{
  itimerspec cleanup {};
  itimerspec old {};

  if (timerfd_gettime(fd, &old) == -1) {
    throw microloop::KernelException(errno);
  }

  if (timerfd_settime(fd, 0, &cleanup, &old) == -1) {
    throw microloop::KernelException(errno);
  }
}

void Timeout::notify()
{
  callback();
}

}  // namespace microloop::event_sources
