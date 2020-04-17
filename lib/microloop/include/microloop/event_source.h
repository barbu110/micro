//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <atomic>
#include <functional>
#include <sys/epoll.h>
#include <tuple>
#include <utility>

namespace microloop
{

class EventLoop;

// FIXME Use the one in event_source_result.h

template <class... ReturnTypeParams>
class TypeHelper
{
public:
  using ReturnType = std::tuple<ReturnTypeParams...>;
  using Callback = std::function<void(ReturnTypeParams...)>;

protected:
  void set_return_object(ReturnType &&obj)
  {
    return_object = obj;  // TODO Should we std::move(obj)?
  }

  void set_return_object(ReturnTypeParams &&... params)
  {
    return_object = std::make_tuple(std::forward<ReturnTypeParams>(params)...);
  }

  const ReturnType &get_return_object() const
  {
    return return_object;
  }

private:
  ReturnType return_object;
};

class EventSource
{
  friend class EventLoop;

public:
  virtual ~EventSource()
  {}

protected:
  EventSource(std::uint32_t fd = 0) : fd{fd}
  {}

  std::uint32_t get_fd() const
  {
    return fd;
  }

  void set_fd(std::uint32_t fd)
  {
    this->fd = fd;
  }

  /**
   * The events that shall be added to the epoll instance interest list for the file descriptor
   * wrapped by this event source.
   * @return If this function returns 0, then the event source is not actually added to the epoll
   * instance, but is treated according to other policies. See the constructor for details.
   */
  virtual std::uint32_t produced_events() const
  {
    return EPOLLIN | EPOLLOUT;
  }

  /**
   * @return Whether this event source can natively be run asychronously or not. If `false` is
   * returned from this function, then `run_callback()` will be submitted to the thread pool
   * instance embedded within the event loop.
   */
  virtual bool native_async() const
  {
    return true;
  }

  /**
   * Function to be executed when the event source is initially added to the event loop. For many
   * event sources this should be used for actually doing their job.
   */
  virtual void start() = 0;

  /**
   * The callback to be executed when the file descriptor is reported to be ready by the `epoll`
   * instance. For some event sources, this is where their job should be executed (e.g. a job that
   * needs to be run every time the file descriptor is ready).
   *
   * @see microloop::event_sources::net::AwaitConnections for an example
   */
  virtual void run_callback()
  {}

private:
  std::uint32_t fd;
};

}  // namespace microloop
