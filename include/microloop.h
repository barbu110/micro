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
static void set_timeout(int ms, microloop::EventLoop *event_loop, Callback on_expired)
{
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  event_loop->add_event_source(new Timer<Callback>(ms, TimerType::TIMEOUT, event_loop, on_expired));
}

template <typename Callback>
static void set_interval(int ms, microloop::EventLoop *event_loop, Callback on_expired)
{
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  event_loop->add_event_source(new Timer<Callback>(ms, TimerType::INTERVAL, event_loop, on_expired));
}

}  // namespace microloop::timers
