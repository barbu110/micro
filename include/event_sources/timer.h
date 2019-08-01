//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "event_source.h"
#include "kernel_exception.h"

#include <chrono>
#include <errno.h>
#include <functional>
#include <ratio>
#include <sys/timerfd.h>
#include <type_traits>
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

class BaseTimer : public virtual microloop::EventSource
{
public:
  /**
   * How many times the timer has expired so far.
   */
  virtual std::uint64_t get_expirations_count() const noexcept = 0;

  /**
   * Set a new value for the timer.
   * @param chrono::nanoseconds The value to be set.
   */
  virtual void set_value(std::chrono::nanoseconds) = 0;

  virtual ~BaseTimer()
  {}
};

class TimerController
{
public:
  TimerController(BaseTimer *timer, microloop::EventLoop *event_loop) :
      timer{timer},
      event_loop{event_loop},
      start_time{std::chrono::high_resolution_clock::now()}
  {}

  /**
   * Cancel the timer.
   */
  void cancel()
  {
    event_loop->remove_event_source(timer);
  }

  /**
   * How many times the timer has expired so far.
   */
  std::uint64_t get_expirations_count() const noexcept
  {
    return timer->get_expirations_count();
  }

  /**
   * How much time passed since the timer controller has been created.
   */
  std::chrono::high_resolution_clock::duration get_elapsed_time() const noexcept
  {
    auto curr_time = std::chrono::high_resolution_clock::now();
    return curr_time - start_time;
  }

  /**
   * Set a new value for the timer.
   * @param chrono::nanoseconds The value to be set.
   */
  void set_value(std::chrono::nanoseconds value)
  {
    timer->set_value(value);
  }

private:
  BaseTimer *timer;
  microloop::EventLoop *event_loop;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

template <class Callback>
class Timer : public BaseTimer
{
public:
  Timer(std::chrono::nanoseconds value, TimerType type, microloop::EventLoop *event_loop,
      Callback callback) :
      microloop::EventSource{},
      value{value},
      type{type},
      callback{callback},
      controller{this, microloop::EventLoop::get_main()}
  {
    static_assert(std::is_invocable_v<Callback, TimerController &>);

    int fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if (fd == -1)
    {
      throw microloop::KernelException(errno);
    }

    set_fd(fd);
    set_value(value);
  }

  ~Timer() override
  {
    close(get_fd());
  }

  /**
   * How many times the timer has expired so far.
   */
  std::uint64_t get_expirations_count() const noexcept override
  {
    return expirations_count;
  }

  /**
   * Set a new value for the timer.
   * @param chrono::nanoseconds The value to be set.
   */
  void set_value(std::chrono::nanoseconds value) override
  {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(value);
    auto fraction_ns = value - seconds;

    timespec t{seconds.count(), fraction_ns.count()};
    itimerspec timer_value{};
    timer_value.it_value = t;

    if (type == TimerType::INTERVAL)
    {
      timer_value.it_interval = t;
    }

    if (timerfd_settime(get_fd(), 0, &timer_value, nullptr) == -1)
    {
      throw microloop::KernelException(errno);
    }

    this->value = value;
  }

protected:
  void start() override
  {}

  void run_callback() override
  {
    /*
     * How many times the timer has expired since the last read(). This will certainly be 1. We
     * perform the read here to consume the event.
     */
    std::uint64_t value = 0;
    ssize_t nread = read(get_fd(), &value, sizeof(std::uint64_t));
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

    expirations_count += value;

    callback(controller);
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
  std::chrono::nanoseconds value;

  /**
   * Type of the timer.
   */
  TimerType type;

  /**
   * Callback to call whenever the timer expires.
   */
  Callback callback;

  /**
   * How many times the timer expired.
   */
  std::uint64_t expirations_count = 0;

  /**
   * The timer controller passed as parameter to the timer callback.
   */
  TimerController controller;
};

}  // namespace microloop::event_sources
