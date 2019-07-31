// Copyright 2019 Victor Barbu.

#pragma once

#include <errno.h>
#include <event_source.h>
#include <functional>
#include <kernel_exception.h>
#include <sys/timerfd.h>
#include <unistd.h>

namespace microloop::event_sources
{

template <class Callback>
class Timeout : public microloop::EventSource
{
public:
  Timeout(int timeout, Callback callback) :
      microloop::EventSource{}, timeout{timeout}, callback{callback}
  {
    int fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if (fd == -1)
    {
      throw microloop::KernelException(errno);
    }

    set_fd(fd);

    itimerspec timer_value{{0, 0}, {timeout / 1000, timeout % 1000 * 1000000}};
    if (timerfd_settime(fd, 0, &timer_value, nullptr) == -1)
    {
      throw microloop::KernelException(errno);
    }
  }

  virtual ~Timeout() override
  {
    close(get_fd());
  }

protected:
  virtual void start() override
  {}

  virtual void run_callback() override
  {
    std::uint64_t expirations_count = 0;
    ssize_t nread = read(get_fd(), &expirations_count, sizeof(std::uint64_t));
    if (nread == -1)
    {
      /*
       * According to the man-page, EAGAIN/EWOULDBLOCK could be returned if no timer expirations
       * have occurred yet. If the application somehow gets in that state, it is certainly an error
       * state, because at the pooint "run_callback" is executed, the timer file descriptor should
       * be readable.
       *
       * See http://man7.org/linux/man-pages/man2/timerfd_create.2.html
       */
      throw microloop::KernelException(errno);
    }

    /*
     * We only read the expirations count to consume the event on the timer as this value is of no
     * importance.
     */

    callback();
  }

  virtual std::uint32_t produced_events() const override
  {
    return EPOLLIN;
  }

private:
  // Timeout in milliseconds.
  int timeout;

  // The callable object to call when the timer finishes.
  Callback callback;
};

}  // namespace microloop::event_sources
