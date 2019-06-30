// Copyright 2019 Victor Barbu

#pragma once

namespace microloop {

class EventLoop;

class EventSource {
  friend class EventLoop;

public:
  struct TrackingData {
    int fd;
    int thread_id = -1;
  };

  virtual ~EventSource()
  {}

protected:
  virtual TrackingData get_tracking_data() const = 0;

  virtual void start() = 0;

  virtual void cleanup() = 0;

  virtual void notify() = 0;
};

}  // namespace microloop
