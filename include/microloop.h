//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "event_loop.h"
#include "event_sources/timer.h"

#include <string>

namespace microloop::timers
{

template <typename Callback>
static void set_timeout(std::chrono::nanoseconds val, EventLoop *event_loop, Callback cb)
{
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  event_loop->add_event_source(new Timer<Callback>(val, TimerType::TIMEOUT, event_loop, cb));
}

template <typename Callback>
static void set_interval(std::chrono::nanoseconds val, EventLoop *event_loop, Callback cb)
{
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  event_loop->add_event_source(new Timer<Callback>(val, TimerType::INTERVAL, event_loop, cb));
}

}  // namespace microloop::timers
