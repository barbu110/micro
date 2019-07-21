//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_source.h>
#include <signal.h>
#include <tuple>
#include <unistd.h>

namespace microloop
{

class EventLoop;

class FsEventSource : public EventSource
{
  friend class EventLoop;
};

#define WORKER_RETURN(...)                                                                         \
  do                                                                                               \
  {                                                                                                \
    return_object = std::make_tuple(__VA_ARGS__);                                                  \
  } while (false)

}  // namespace microloop
