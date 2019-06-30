// Copyright 2019 Victor Barbu

#pragma once

#include <event_source_helper.h>
#include <memory>
#include <string>
#include <thread_event_source.h>
#include <tuple>
#include <unistd.h>

namespace microloop::event_sources::filesystem {

class Write : public microloop::ThreadEventSource {
  using TypeHelper = microloop::detail::EventSourceHelper<ssize_t>;

public:
  Write(const std::string &filename, const std::string &buffer, TypeHelper::Callback callback);
  void start() override;
  void cleanup() override;
  void notify() override;

private:
  std::string filename;
  const std::string &buffer;
  TypeHelper::Callback callback;
  TypeHelper::Result return_object;
};

#define MICROLOOP_FS_WRITE(filename, buffer, callback)                                             \
  do {                                                                                             \
    microloop::EventLoop::get_main()->add_event_source(                                            \
        new microloop::event_sources::filesystem::Write(filename, buffer, callback));              \
  } while (false)

}  // namespace microloop::event_sources::filesystem
