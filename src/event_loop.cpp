// Copyright 2019 Victor Barbu

#include <errno.h>
#include <event_loop.h>
#include <event_source.h>
#include <iostream>
#include <kernel_exception.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <thread_event_source.h>
#include <unistd.h>
#include <vector>

namespace microloop {

EventLoop *EventLoop::main_instance = nullptr;

EventLoop *EventLoop::get_main()
{
  if (main_instance == nullptr) {
    main_instance = new EventLoop;
  }

  return main_instance;
}

EventLoop::EventLoop() : signals_monitor{}
{
  epollfd = epoll_create(1);
  if (epollfd == -1) {
    throw KernelException(errno);
  }

  add_event_source(&signals_monitor);
}

EventLoop::~EventLoop()
{
  close(epollfd);
}

void EventLoop::add_event_source(EventSource *event_source)
{
  event_source->start();

  auto [fd, thread_id] = event_source->get_tracking_data();

  std::cout << __FUNCTION__ << ": " << fd << " " << thread_id << "\n";

  if (fd != -1) {
    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
      throw microloop::KernelException(errno);
    }

    event_sources[fd] = event_source;
  } else {  // We have a thread_event_source
    auto thread_event_source = static_cast<ThreadEventSource *>(event_source);
    thread_event_source->set_thread_id(next_thread_id());

    thread_event_sources[thread_event_source->get_thread_id()] = thread_event_source;
  }
}

int EventLoop::next_thread_id() const
{
  return thread_event_sources.size();
}

bool EventLoop::next_tick()
{
  epoll_event events_list[128];

  auto ready = epoll_wait(epollfd, events_list, 128, -1);
  if (ready < 0) {
    return false;
  }

  for (int i = 0; i < ready; i++) {
    auto &event = events_list[i];

    if (event.events & EPOLLIN) {
      auto fd = event.data.fd;
      if (fd == signals_monitor.get_tracking_data().fd) {
        signalfd_siginfo signal_info{};
        if (read(fd, &signal_info, sizeof(signalfd_siginfo)) == -1) {
          throw KernelException(errno);
        }

        auto thread_id = signal_info.ssi_int;

        auto thread_event_source = thread_event_sources[thread_id];
        thread_event_source->notify();
      } else {
        auto event_source = event_sources[fd];

        event_source->notify();
        event_source->cleanup();
      }
    }
  }

  return true;
}

}  // namespace microloop
