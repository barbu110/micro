//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_loop.h>
#include <event_sources/filesystem/read.h>
#include <event_sources/filesystem/write.h>
#include <event_sources/timeout.h>
#include <string>

namespace microloop::timers {

static void set_timeout(microloop::event_sources::Timeout::callback_type on_done, int ms)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::Timeout(on_done, ms));
}

}  // namespace microloop::timers

namespace microloop::fs {

template <typename Filename, typename Buffer, typename Callback>
static void write(const Filename &filename, const Buffer &buffer, Callback callback)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::filesystem::Write(filename, buffer, callback));
}

template <typename Filename, typename Callback>
static void read(const Filename &filename, Callback callback)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::filesystem::Read(filename, callback));
}

template <typename Filename, typename MaxLen, typename Callback>
static void read(const Filename &filename, MaxLen max_len, Callback callback)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::filesystem::Read(filename, max_len, callback));
}

template <typename Filename, typename MaxLen, typename Offset, typename Callback>
static void read(const Filename &filename, MaxLen max_len, Offset offset, Callback callback)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::filesystem::Read(filename, max_len, offset, callback));
}

}  // namespace microloop::fs
