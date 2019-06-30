//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include <errno.h>
#include <event_sources/filesystem/read.h>
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

Read::Read(const std::string &filename, TypeHelper::Callback callback) :
    Read{filename, 0, 0, callback}
{}

Read::Read(const std::string &filename, size_t max_len, TypeHelper::Callback callback) :
    Read{filename, max_len, 0, callback}
{}

Read::Read(const std::string &filename, size_t max_len, off_t offset, Read::TypeHelper::Callback callback) :
    microloop::ThreadEventSource{},
    filename{filename}, max_len{max_len}, offset{offset}, callback{callback}
{}

void Read::start()
{
  std::thread worker{[&]() {
    int fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK);

    if (fd == -1) {
      throw KernelException(errno);
    }

    std::string file_contents;
    if (max_len) {
      file_contents.reserve(max_len);
    }

    auto buf_size = max_len ? max_len : 4096;
    auto buf = std::make_unique<char[]>(buf_size);

    ssize_t read_count = pread(fd, buf.get(), buf_size, offset);
    if (read_count > 0) {
      file_contents.append(buf.get(), read_count);
    } else if (read_count < 0) {
      throw KernelException(errno);
    }

    return_object = std::make_tuple(file_contents);

    sigval signal_data{0};
    signal_data.sival_int = get_thread_id();

    sigqueue(getpid(), SIGUSR1, signal_data);
  }};

  worker.detach();
}

void Read::cleanup()
{}

void Read::notify()
{
  std::apply(callback, return_object);
}

}  // namespace microloop::event_sources::filesystem
