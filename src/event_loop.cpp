// Copyright 2019 Victor Barbu

#include <event_loop.h>

#include <vector>

#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <event_source.h>
#include <kernel_exception.h>

namespace microloop {

EventLoop *EventLoop::main_instance = nullptr;

EventLoop *EventLoop::get_main()
{
  if (main_instance == nullptr) {
    main_instance = new EventLoop;
  }

  return main_instance;
}

EventLoop::EventLoop()
{
  epollfd = epoll_create(1);
  if (epollfd == -1) {
    throw KernelException(errno);
  }
}

EventLoop::~EventLoop() {
  close(epollfd);
}

void EventLoop::add_event_source(EventSource *event_source)
{
  auto fd = event_source->get_fd();

  epoll_event ev{};
  ev.events = EPOLLIN | EPOLLOUT;
  ev.data.fd = fd;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    throw microloop::KernelException(errno);
  }

  event_sources[fd] = event_source;
}

bool EventLoop::next_tick()
{
  epoll_event events_list[128];

  auto ready = epoll_wait(epollfd, events_list, 128, -1);
  if (ready < 0) {
    return false;
  }

  for (int i = 0; i < ready; i++) {
    auto& event = events_list[i];

    if (event.events & EPOLLIN) {
      auto fd = event.data.fd;
      auto event_source = event_sources[fd];

      event_source->notify();
      event_source->cleanup();
    }
  }

  return true;
}

}  // namespace microloop
