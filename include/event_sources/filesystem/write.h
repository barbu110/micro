// Copyright 2019 Victor Barbu

#pragma once

#include <errno.h>
#include <fcntl.h>
#include <fs_event_source.h>
#include <functional>
#include <iostream>
#include <kernel_exception.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <tuple>
#include <unistd.h>

namespace microloop::event_sources::filesystem
{

class Write : public microloop::FsEventSource
{
  using Types = microloop::TypeHelper<ssize_t>;

public:
  Write(const std::string &filename, const std::string &buffer, Types::Callback callback) :
      microloop::FsEventSource{},
      has_filename{true}, filename{filename}, buffer{buffer}, callback{callback}
  {}

  Write(int fd, const std::string &buffer, Types::Callback callback) :
      has_filename{false}, fd{fd}, buffer{buffer}, callback{callback}
  {}

  ~Write()
  {
    close(fd);
  }

  void start() override
  {
    std::thread worker{[&]() {
      if (has_filename)
      {
        fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_APPEND | O_NONBLOCK);
      }

      if (fd == -1)
      {
        throw KernelException(errno);
      }

      ssize_t written = write(fd, buffer.c_str(), buffer.size());
      if (written != buffer.size())
      {
        throw KernelException(errno);
      }

      WORKER_RETURN(written);
    }};

    worker.detach();
  }

  void run_callback() override
  {
    std::apply(callback, return_object);
  }

private:
  bool has_filename;
  std::string filename;
  int fd = -1;
  const std::string &buffer;
  Types::Callback callback;
  Types::ReturnType return_object;
};

}  // namespace microloop::event_sources::filesystem
