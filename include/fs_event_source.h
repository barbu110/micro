//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_source.h>
#include <signal.h>
#include <tuple>
#include <unistd.h>

namespace microloop {

class EventLoop;

class FsEventSource : public EventSource {
  friend class EventLoop;

protected:
  bool has_fd() const override
  {
    return false;
  }

  inline void signal_ready() const
  {
    sigval signal_data{0};
    signal_data.sival_int = get_id();

    sigqueue(getpid(), SIGUSR1, signal_data);
  }
};

#define WORKER_RETURN(...)                                                                         \
  do {                                                                                             \
    return_object = std::make_tuple(__VA_ARGS__);                                                  \
    signal_ready();                                                                                \
  } while (false)

}  // namespace microloop
