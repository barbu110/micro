// Copyright 2019 Victor Barbu

#pragma once

#include <map>
#include <signals_monitor.h>

namespace microloop {

class EventSource;
class ThreadEventSource;

class EventLoop {
private:
  EventLoop();

public:
  static EventLoop* get_main();

  void add_event_source(EventSource* event_source);

  bool next_tick();

  ~EventLoop();

private:
  int next_thread_id() const;

private:
  int epollfd;
  SignalsMonitor signals_monitor;
  std::map<int /* file descriptor */, EventSource*> event_sources;
  std::map<int /* thread_id */, ThreadEventSource*> thread_event_sources;

  static EventLoop* main_instance;
};

}  // namespace microloop

#define MICROLOOP_TICK() microloop::EventLoop::get_main()->next_tick()
