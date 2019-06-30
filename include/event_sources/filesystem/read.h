// Copyright 2019 Victor Barbu

#pragma once

#include <event_source_helper.h>
#include <memory>
#include <string>
#include <thread_event_source.h>
#include <tuple>
#include <unistd.h>

namespace microloop::event_sources::filesystem {

class Read : public microloop::ThreadEventSource {
  using TypeHelper = microloop::detail::EventSourceHelper<std::string>;

public:
  Read(const std::string &filename, TypeHelper::Callback callback);
  Read(const std::string &filename, size_t max_len, TypeHelper::Callback callback);
  Read(const std::string &filename, size_t max_len, off_t offset, TypeHelper::Callback callback);
  void start() override;
  void cleanup() override;
  void notify() override;

private:
  std::string filename;
  size_t max_len;
  off_t offset;
  TypeHelper::Callback callback;
  TypeHelper::Result return_object;
};

#define MICROLOOP_FS_READ(filename, max_len, callback)                                             \
  do {                                                                                             \
    microloop::EventLoop::get_main()->add_event_source(                                            \
        new microloop::event_sources::filesystem::Read(filename, max_len, callback));              \
  } while (false)

}  // namespace microloop::event_sources::filesystem
