// Copyright 2019 Victor Barbu.

#pragma once

#include <functional>

#include <event_source.h>

namespace microloop::event_sources {

class Timeout : public microloop::EventSource {
public:
  using callback_type = std::function<void()>;

  Timeout(callback_type callback, int timeout);

  virtual ~Timeout() override;

protected:
  virtual int get_fd() override;

  virtual void cleanup() override;

  virtual void notify() override;

private:
  // The file descriptor of the timer
  int fd;

  // Timeout in milliseconds.
  int timeout;

  // The callable object to call when the timer finishes.
  callback_type callback;
};

}  // namespace microloop::event_sources

#define MICROLOOP_SET_TIMEOUT(on_done, ms) \
  microloop::EventLoop::get_main()->add_event_source( \
    new microloop::event_sources::Timeout(on_done, ms) \
  );
