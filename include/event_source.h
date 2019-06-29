// Copyright 2019 Victor Barbu

#pragma once

namespace microloop {

class EventLoop;

class EventSource {
  friend class EventLoop;

  public:
  virtual ~EventSource() {}

  protected:
  virtual int get_fd() = 0;

  virtual void cleanup() = 0;

  virtual void notify() = 0;
};

}  // namespace microloop
