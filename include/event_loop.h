//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <errno.h>
#include <kernel_exception.h>
#include <limits>
#include <map>
#include <signal.h>
#include <signals_monitor.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <iostream>

namespace microloop {

class EventSource;
class ThreadEventSource;

class EventLoop {
  EventLoop()
  {
    epollfd = epoll_create(1);
    if (epollfd == -1) {
      throw KernelException(errno);
    }

    add_event_source(&signals_monitor);
  }

public:
  static EventLoop *get_main()
  {
    if (main_instance == nullptr) {
      main_instance = new EventLoop;
    }

    return main_instance;
  }

  void add_event_source(EventSource *event_source)
  {
    event_source->start();

    std::uint64_t key = event_source->get_id();
    if (!event_source->has_fd()) {
      event_source->set_id(next_thread_id());
      key = event_source->get_id() << 4;
    }

    if (event_source->has_fd()) {
      epoll_event ev{};
      ev.events = EPOLLIN | EPOLLOUT;
      ev.data.fd = event_source->get_id();

      if (epoll_ctl(epollfd, EPOLL_CTL_ADD, key, &ev) == -1) {
        throw microloop::KernelException(errno);
      }
    }

    event_sources[key] = event_source;
  }

  bool register_signal_handler(SignalsMonitor::SignalHandler callback)  // TODO
  {
    return false;
  }

  bool next_tick()
  {
    epoll_event events_list[128];

    auto ready = epoll_wait(epollfd, events_list, 128, -1);
    if (ready < 0) {
      return false;
    }

    for (int i = 0; i < ready; i++) {
      auto &event = events_list[i];

      auto key = event.data.fd;

      std::cout << "Got event from " << key << "\n";

      if (key == signals_monitor.get_id()) {
        signalfd_siginfo signal_info{};
        if (read(key, &signal_info, sizeof(signalfd_siginfo)) == -1) {
          throw KernelException(errno);
        }

        auto thread_id = signal_info.ssi_int;
        key = thread_id << 4;
      }

      auto event_source = event_sources[key];

      event_source->run_callback();

      delete event_source;
      event_sources.erase(key);  // TODO Check that this calls the destructor.
    }

    return true;
  }

  ~EventLoop()
  {
    close(epollfd);
  }

private:
  int next_thread_id() const
  {
    static const auto uint32_max = std::numeric_limits<std::uint32_t>::max();
    return event_sources.size() % uint32_max;
  }

private:
  int epollfd;
  SignalsMonitor signals_monitor;
  std::map<std::uint64_t, EventSource *> event_sources;

  static EventLoop *main_instance;
};

}  // namespace microloop

#define MICROLOOP_TICK() microloop::EventLoop::get_main()->next_tick()
