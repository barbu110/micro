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
static void set_timeout(int ms, Callback on_expired)
{
  using microloop::EventLoop;
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  EventLoop::get_main()->add_event_source(new Timer(ms, TimerType::TIMEOUT, on_expired));
}

template <typename Callback>
static void set_interval(int ms, Callback on_expired)
{
  using microloop::EventLoop;
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  EventLoop::get_main()->add_event_source(new Timer(ms, TimerType::INTERVAL, on_expired));
}

}  // namespace microloop::timers
