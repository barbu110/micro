// Copyright 2019 Victor Barbu

#pragma once

#include <map>
#include <sys/epoll.h>

namespace microloop {

class EventSource;

class EventLoop {
  private:
  EventLoop();

  public:
  static EventLoop* get_main();

  void add_event_source(EventSource* event_source);

  bool next_tick();

  ~EventLoop();

  private:
  int epollfd;
  std::map<int, EventSource*> event_sources;

  static EventLoop* main_instance;
};

}  // namespace microloop

#define MICROLOOP_TICK() \
  microloop::EventLoop::get_main()->next_tick()
