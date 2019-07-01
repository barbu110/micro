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

class ThreadEventSource : public EventSource {
  friend class EventLoop;

public:
  int get_thread_id() const
  {
    return thread_id;
  }

protected:
  EventSource::TrackingData get_tracking_data() const override
  {
    return {-1, thread_id};
  }

  inline void signal_ready() const
  {
    sigval signal_data{0};
    signal_data.sival_int = get_thread_id();

    sigqueue(getpid(), SIGUSR1, signal_data);
  }

private:
  void set_thread_id(int thread_id)
  {
    this->thread_id = thread_id;
  }

private:
  int thread_id;
};

#define WORKER_RETURN(...)                                                                         \
  do {                                                                                             \
    return_object = std::make_tuple(__VA_ARGS__);                                                  \
    signal_ready();                                                                                \
  } while (false)

}  // namespace microloop
