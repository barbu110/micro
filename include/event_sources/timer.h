//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "event_source.h"
#include "kernel_exception.h"

#include <errno.h>
#include <functional>
#include <sys/timerfd.h>
#include <unistd.h>

namespace microloop::event_sources
{

enum class TimerType
{
  /**
   * The timer is set to expire only once, then destroy itself.
   */
  TIMEOUT,

  /**
   * The timer will expire at the specified interval.
   */
  INTERVAL,
};

template <class Callback>
class Timer : public microloop::EventSource
{
public:
  Timer(int value, TimerType type, Callback callback) :
      microloop::EventSource{}, value{value}, type{type}, callback{callback}
  {
    int fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if (fd == -1)
    {
      throw microloop::KernelException(errno);
    }

    set_fd(fd);

    timespec t{value / 1000, value % 1000 * 1000000};
    itimerspec timer_value{};
    timer_value.it_value = t;

    if (type == TimerType::INTERVAL)
    {
      timer_value.it_interval = t;
    }

    if (timerfd_settime(fd, 0, &timer_value, nullptr) == -1)
    {
      throw microloop::KernelException(errno);
    }
  }

  ~Timer() override
  {
    close(get_fd());
  }

protected:
  void start() override
  {}

  void run_callback() override
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

  std::uint32_t produced_events() const override
  {
    switch (type)
    {
    case TimerType::TIMEOUT:
      return EPOLLIN | EPOLLONESHOT;
    case TimerType::INTERVAL:
      return EPOLLIN;
    }
  }

private:
  /**
   * Timer value in milliseconds.
   */
  int value;

  /**
   * Type of the timer.
   */
  TimerType type;

  /**
   * Callback to call whenever the timer expires.
   */
  Callback callback;
};

}  // namespace microloop::event_sources
