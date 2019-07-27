//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <buffer.h>
#include <event_loop.h>
#include <event_sources/timeout.h>
#include <string>

namespace microloop::timers
{

template <typename Callback>
static void set_timeout(int ms, Callback on_done)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::Timeout(ms, on_done));
}

}  // namespace microloop::timers
