//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "event_loop.h"
#include "event_sources/timer.h"

#include <string>

namespace microloop::timers
{

/**
 * Run the given callback after the given amount of time, once.
 * @param val The duration of the timer
 * @param event_loop The event loop which will watch the timer
 * @param cb The callback to be executed when the timer expires. Bote that this callback
 * must implement the following signature:
 *
 *     void(microloop::event_sources::TimerController &);
 *
 * The timer controller can be used to retrieve information about the timer and alter its
 * behavior (e.g. cancel it, change its expiration interval).
 */
template <typename Callback>
static void set_timeout(std::chrono::nanoseconds val, EventLoop *event_loop, Callback cb)
{
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  event_loop->add_event_source(new Timer<Callback>(val, TimerType::TIMEOUT, event_loop, cb));
}

/**
 * Run the given callback at the specified interval.
 * @param val The interval at which the timer will expire
 * @param event_loop The event loop that watches the timer
 * @param cb The callback to be executed every time the timer expires. Note that this callback
 * must implement the following signature:
 *
 *     void(microloop::event_sources::TimerController &);
 *
 * The timer controller can be used to retrieve information about the timer and alter its
 * behavior (e.g. cancel it, change its expiration interval).
 */
template <typename Callback>
static void set_interval(std::chrono::nanoseconds val, EventLoop *event_loop, Callback cb)
{
  using microloop::event_sources::Timer;
  using microloop::event_sources::TimerType;

  event_loop->add_event_source(new Timer<Callback>(val, TimerType::INTERVAL, event_loop, cb));
}

}  // namespace microloop::timers
