//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include <errno.h>
#include <event_sources/filesystem/write.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <kernel_exception.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

namespace microloop::event_sources::filesystem {

Write::Write(
    const std::string &filename, const std::string &buffer, Write::TypeHelper::Callback callback) :
    microloop::ThreadEventSource{},
    filename{filename}, buffer{buffer}, callback{callback}
{}

void Write::start()
{
  std::thread worker{[&]() {
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_APPEND | O_NONBLOCK);

    if (fd == -1) {
      throw KernelException(errno);
    }

    ssize_t written = write(fd, buffer.c_str(), buffer.size());
    if (written != buffer.size()) {
      throw KernelException(errno);
    }

    return_object = std::make_tuple(written);

    sigval signal_data{0};
    signal_data.sival_int = get_thread_id();

    sigqueue(getpid(), SIGUSR1, signal_data);
  }};

  worker.detach();
}

void Write::cleanup()
{}

void Write::notify()
{
  std::apply(callback, return_object);
}

}  // namespace microloop::event_sources::filesystem
