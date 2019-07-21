//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <errno.h>
#include <event_source.h>
#include <kernel_exception.h>
#include <limits>
#include <map>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <utils/thread_pool.h>
#include <stdexcept>
#include <sys/socket.h>
#include <vector>
#include <signals_monitor.h>

namespace microloop {

class EventLoop {
  EventLoop() : thread_pool{4}
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

  utils::ThreadPool &get_thread_pool()
  {
    return thread_pool;
  }

  void add_event_source(EventSource *event_source)
  {
    std::uint32_t fd = event_source->get_fd();

    auto produced_events = event_source->produced_events();
    if (!produced_events) {
      throw std::invalid_argument("produced_events is 0");
    }

    epoll_event ev{};
    ev.events = produced_events;
    ev.data.ptr = static_cast<void *>(event_source);

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
      throw microloop::KernelException(errno);
    }

    event_sources[fd] = event_source;  // TODO Do we still need storing event soucres in the hashtable?

    if (event_source->native_async()) {
      event_source->start();
    } else {
      thread_pool.submit(&EventSource::start, event_source);
    }
  }

  void remove_event_source(EventSource *event_source)
  {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, event_source->get_fd(), nullptr) == -1) {
      throw KernelException(errno);
    }

    event_sources.erase(event_source->get_fd());
    delete event_source;
  }

  inline void register_signal_handler(std::uint32_t sig, SignalsMonitor::SignalHandler &&callback)
  {
    signals_monitor.register_signal_handler(sig, std::move(callback));
  }

  bool next_tick()
  {
    epoll_event events_list[32];

    auto ready = epoll_wait(epollfd, events_list, 32, -1);
    if (ready < 0) {
      return false;
    }

    for (int i = 0; i < ready; i++) {
      auto &event = events_list[i];

      auto event_source = reinterpret_cast<EventSource *>(event.data.ptr);

      if (event_source->needs_retry()) {
        thread_pool.submit(&EventSource::start, event_source);
      } else {
        event_source->run_callback();
      }
    }

    return true;
  }

  ~EventLoop()
  {
    close(epollfd);
  }

private:
  int epollfd;
  utils::ThreadPool thread_pool;
  SignalsMonitor signals_monitor;
  std::map<std::uint64_t, EventSource *> event_sources;

  static EventLoop *main_instance;
};

}  // namespace microloop

#define MICROLOOP_TICK() microloop::EventLoop::get_main()->next_tick()
