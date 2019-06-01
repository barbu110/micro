// Copyright 2019 Victor Barbu

#pragma once

namespace microloop {

class EventLoop {
public:
  virtual void nextTick() = 0;
  virtual ~EventLoop() = 0;
};

}  // namespace microloop
