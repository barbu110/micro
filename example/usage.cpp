// Copyright 2019 Victor Barbu

#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <functional>

#include <iostream>
#include <map>

#include "kernel_exception.h"

class Source;

using EventLoopCallback = std::function<void(Source&)>;

class Source {
public:
  EventLoopCallback callback;

public:
  Source(EventLoopCallback callback): callback{callback} {}
  virtual ~Source() {}

  virtual void start(int epoll_fd) = 0;

  virtual void cleanup() {}
};

class SourceCallbackWrapper {
  Source& source_;

public:
  SourceCallbackWrapper(Source& source): source_{source} {}

  void operator()() {
    source_.callback(source_);
    source_.cleanup();
  }
};

struct TimeoutSource : public Source {
  int timeout_s;
  int fd;

  TimeoutSource(int timeout_s, EventLoopCallback callback)
      : Source{callback}, timeout_s{timeout_s} {
    fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1) {
      throw microloop::KernelException(errno);
    }

    itimerspec timer_value{{0, 0}, {timeout_s, 0}};
    if (timerfd_settime(fd, 0, &timer_value, nullptr) == -1) {
      throw microloop::KernelException(errno);
    }
  }

  ~TimeoutSource() { close(fd); }

  void start(int epoll_fd) override {
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
      throw microloop::KernelException(errno);
    }
  }

  void cleanup() override {
    itimerspec cleanup{};
    itimerspec old{};

    if (timerfd_gettime(fd, &old) == -1) {
      throw microloop::KernelException(errno);
    }

    if (timerfd_settime(fd, 0, &cleanup, &old) == -1) {
      throw microloop::KernelException(errno);
    }
  }
};

struct FileWriteSource : public Source {
  std::string filename;
  int fd;
  std::string buffer;

  FileWriteSource(const std::string& filename, const std::string& buffer,
                  EventLoopCallback callback):
      Source{callback}, filename{filename}, buffer{buffer} {
    fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_NONBLOCK | O_RDWR, S_IRWXU);
    if (fd == -1) {
      throw microloop::KernelException(errno);
    }
  }

  void start(int epoll_fd) override {
    auto written = write(fd, buffer.c_str(), buffer.size());
    std::cout << "written value " << written << "\n";
    std::cout << "fd " << fd << "\n";
    std::cout << "errno: " << strerror(errno) << "\n";
  }

  void cleanup() override {
    close(fd);
  }
};

int main() {
  std::cout << "PID=" << getpid() << "\n";

  auto epoll_fd = epoll_create(1);  // The size parameter is ignored since
                                    // Linux 2.6.8
  if (epoll_fd == -1) {
    std::cerr << strerror(errno);
    return errno;
  }

  std::map<int /* fd */, Source*> sources;

  auto make_callback = [](std::string source_name) {
    return [=](Source&) {
      std::cout << source_name << " finished\n";
    };
  };

  auto t1 = new TimeoutSource{10, make_callback("timeout 1")};
  auto t2 = new TimeoutSource{5, make_callback("timeout 2")};
  auto t3 = new TimeoutSource{2, make_callback("timeout 3")};
  auto t4 = new TimeoutSource{1, make_callback("timeout 4")};

  std::string buf1(1024 * 4096, '0');
  auto fw1 = new FileWriteSource{"./data1.bin", buf1, make_callback("fw 1")};

  std::string buf2(1024 * 2048, '0');
  auto fw2 = new FileWriteSource{"./data2.bin", buf2, make_callback("fw 2")};

  sources[t1->fd] = t1;
  sources[t2->fd] = t2;
  sources[t3->fd] = t3;
  sources[t4->fd] = t4;
  sources[fw1->fd] = fw1;
  sources[fw2->fd] = fw2;

  for (const auto& source : sources) {
    source.second->start(epoll_fd);
  }

  struct epoll_event events_list[100];
  while (true) {
    auto ready = epoll_wait(epoll_fd, events_list, 100, -1);

    for (int i = 0; i < ready; i++) {
      auto& event = events_list[i];

      if (event.events & EPOLLIN) {
        SourceCallbackWrapper wrapper{*sources[event.data.fd]};
        wrapper();
      }
    }
  }

  close(epoll_fd);
  return 0;
}
