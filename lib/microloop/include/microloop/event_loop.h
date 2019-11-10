//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microloop/event_source.h"
#include "microloop/signals_monitor.h"
#include "microloop/utils/thread_pool.h"

#include <cstdint>
#include <map>
#include <memory>
#include <unistd.h>

namespace microloop
{

class EventLoop
{
private:
  /**
   * Create an instance of the event loop. The thread pool is initialized with 4 threads by default.
   */
  EventLoop();

public:
  /**
   * @return An instance of the event loop.
   */
  static EventLoop &get_main();

  EventLoop(const EventLoop &) = delete;
  EventLoop &operator=(const EventLoop &) = delete;

  /**
   * @return The embedded thread pool.
   */
  utils::ThreadPool &get_thread_pool()
  {
    return thread_pool;
  }

  /**
   * Add a new event source to the event loop.
   * @param event_source The event source to be added.
   */
  void add_event_source(EventSource *event_source);

  /**
   * Remove an existing event source from the event loop.
   * @param event_source The event source to remove.
   */
  void remove_event_source(EventSource *event_source);

  /**
   * Register a new signal handler.
   * @param sig The signal that the handler responds to. This signal will be blocked via the
   * process sigmask.
   * @param callback The callback to be called when signal `sig` is caught.
   */
  inline void register_signal_handler(std::uint32_t sig, SignalsMonitor::SignalHandler &&callback)
  {
    signals_monitor().register_signal_handler(sig, std::move(callback));
  }

  /**
   * Run the next tick of the event loop. This is responsible for waiting for the registered
   * event sources to trigger, signals to be caught and callbacks to be called for each of the
   * aforementioned events.
   * @return Returns `true` if the event loop should continue its execution, `false` otherwise.
   */
  bool next_tick();

  ~EventLoop()
  {
    close(epollfd);
  }

private:
  SignalsMonitor &signals_monitor()
  {
    return *static_cast<SignalsMonitor *>(event_sources[signals_monitor_fd_].get());
  }

  std::uint32_t epollfd;
  utils::ThreadPool thread_pool;
  std::uint64_t signals_monitor_fd_;
  std::map<std::uint64_t, std::unique_ptr<EventSource>> event_sources;
};

}  // namespace microloop

#define MICROLOOP_TICK() microloop::EventLoop::get_main().next_tick()
