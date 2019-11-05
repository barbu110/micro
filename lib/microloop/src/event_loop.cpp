// Copyright 2019 Victor Barbu

#include "microloop/event_loop.h"

#include "microloop/kernel_exception.h"

#include <errno.h>
#include <sys/epoll.h>
#include <iostream>

namespace microloop
{

EventLoop *EventLoop::main_instance = nullptr;

EventLoop::EventLoop() : thread_pool{4}
{
  std::int32_t fd = epoll_create(1);
  if (fd == -1)
  {
    throw KernelException(errno);
  }

  epollfd = epoll_create(1);

  add_event_source(&signals_monitor);
}

EventLoop *EventLoop::get_main()
{
  if (main_instance == nullptr)
  {
    main_instance = new EventLoop;
  }

  return main_instance;
}

void EventLoop::add_event_source(EventSource *event_source)
{
  std::uint32_t fd = event_source->get_fd();

  auto produced_events = event_source->produced_events();
  if (!produced_events)
  {
    throw std::invalid_argument("the event source produces no events");
  }

  epoll_event ev{};
  ev.events = produced_events;
  ev.data.ptr = static_cast<void *>(event_source);

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
  {
    throw KernelException(errno);
  }

  event_sources[fd]
      = event_source;  // TODO Do we still need storing event soucres in the hashtable?

  if (event_source->native_async())
  {
    event_source->start();
  }
  else
  {
    thread_pool.submit(&EventSource::start, event_source);
  }
}

void EventLoop::remove_event_source(EventSource *event_source)
{
  if (epoll_ctl(epollfd, EPOLL_CTL_DEL, event_source->get_fd(), nullptr) == -1)
  {
    throw KernelException(errno);
  }

  event_sources.erase(event_source->get_fd());
  delete event_source;
}

bool EventLoop::next_tick()
{
  epoll_event events_list[32];

  auto ready = epoll_pwait(epollfd, events_list, 32, -1, &signals_monitor.get_sigmask());
  if (ready < 0)
  {
    return false;
  }

  for (int i = 0; i < ready; i++)
  {
    auto &event = events_list[i];
    auto event_source = reinterpret_cast<EventSource *>(event.data.ptr);
    auto delete_event_source = false;
    if (event_source->produced_events() & EPOLLONESHOT)
    {
      delete_event_source = true;
    }

    /*
     * The callback of an event source may remove it from the event loop leaving us with a dangling
     * pointer here. Thus we need to perform the check of produced events before running the
     * callback while we know for sure the address of the event source is still valid.
     */
    event_source->run_callback();

    if (delete_event_source)
    {
      delete event_source;
    }
  }

  return true;
}

}  // namespace microloop
