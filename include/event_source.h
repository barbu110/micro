//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <atomic>
#include <functional>
#include <sys/epoll.h>
#include <tuple>

namespace microloop {

class EventLoop;

// FIXME Use the one in event_source_result.h

template <class... ReturnTypeParams>
class TypeHelper {
public:
  using ReturnType = std::tuple<ReturnTypeParams...>;
  using Callback = std::function<void(ReturnTypeParams...)>;
};

class EventSource {
  friend class EventLoop;

public:
  virtual ~EventSource()
  {}

protected:
  EventSource(std::uint32_t fd = 0) : fd{fd}, complete{false}
  {}

  std::uint32_t get_fd() const
  {
    return fd;
  }

  void set_fd(std::uint32_t fd)
  {
    this->fd = fd;
  }

  virtual std::uint32_t produced_events() const
  {
    return EPOLLIN | EPOLLOUT;
  }

  virtual bool native_async() const
  {
    return true;
  }

  virtual void start() = 0;

  virtual bool needs_retry() const
  {
    return false;
  }

  virtual void run_callback()
  {}

  bool is_complete() const
  {
    return complete;
  }

protected:
  void mark_complete()
  {
    complete = true;
  }

private:
  std::uint32_t fd;
  std::atomic_bool complete;
};

}  // namespace microloop
