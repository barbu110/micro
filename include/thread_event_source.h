//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_source.h>

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

private:
  void set_thread_id(int thread_id)
  {
    this->thread_id = thread_id;
  }

private:
  int thread_id;
};

}
