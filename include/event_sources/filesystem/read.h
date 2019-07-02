// Copyright 2019 Victor Barbu

#pragma once

#include <memory>
#include <string>
#include <fs_event_source.h>
#include <tuple>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <kernel_exception.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>

namespace microloop::event_sources::filesystem {

class Read : public microloop::FsEventSource {
  using Types = microloop::TypeHelper<std::string>;

public:
  Read(const std::string &filename, Types::Callback callback) :
      Read{filename, 0, 0, callback}
  {}

  Read(const std::string &filename, size_t max_len, Types::Callback callback) :
      Read{filename, max_len, 0, callback}
  {}

  Read(const std::string &filename, size_t max_len, off_t offset, Types::Callback callback) :
      microloop::FsEventSource{},
      filename{filename}, max_len{max_len}, offset{offset}, callback{callback}
  {
    fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
      throw KernelException(errno);
    }
  }

  void start() override
  {
    std::thread worker{[&]() {
      std::string file_contents;
      if (max_len) {
        file_contents.reserve(max_len);
      }

      auto buf_size = max_len ? max_len : 4096;
      auto buf = std::make_unique<char[]>(buf_size);

      ssize_t read_count;
      if (offset) {
        read_count = pread(fd, buf.get(), buf_size, offset);
      } else {
        read_count = read(fd, buf.get(), buf_size);  // We use this to support sockets, pipes etc.
      }

      if (read_count > 0) {
        file_contents.append(buf.get(), read_count);
      } else if (read_count < 0) {
        throw KernelException(errno);
      }

      WORKER_RETURN(file_contents);
    }};

    worker.detach();
  }

  void run_callback() override
  {
    std::apply(callback, return_object);
  }

private:
  std::string filename;
  int fd = -1;
  bool has_filename = true;

  size_t max_len;
  off_t offset;
  Types::Callback callback;
  Types::ReturnType return_object;
};

#define MICROLOOP_FS_READ(filename, max_len, callback)                                             \
  do {                                                                                             \
    microloop::EventLoop::get_main()->add_event_source(                                            \
        new microloop::event_sources::filesystem::Read(filename, max_len, callback));              \
  } while (false)

}  // namespace microloop::event_sources::filesystem
