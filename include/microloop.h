//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <buffer.h>
#include <event_loop.h>
#include <event_sources/filesystem/read.h>
#include <event_sources/filesystem/write.h>
#include <event_sources/net/send.h>
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

namespace microloop::fs
{

template <typename Filename, typename Buffer, typename Callback>
static void write(const Filename &filename, const Buffer &buffer, Callback callback)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::filesystem::Write(filename, buffer, callback));
}

template <typename Buffer, typename Callback>
static void write(int fd, const Buffer &buffer, Callback callback)
{
  microloop::EventLoop::get_main()->add_event_source(
      new microloop::event_sources::filesystem::Write(fd, buffer, callback));
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

namespace microloop::net::utils
{

static ssize_t send(int sock, const Buffer &buf)
{
  ssize_t total_sent = 0;

  auto buf_data = reinterpret_cast<std::uint8_t *>(buf.data());

  do
  {
    ssize_t sent = ::send(sock, buf_data + total_sent, buf.size() - total_sent, 0);
    if (sent == -1 && errno != EWOULDBLOCK && errno != EAGAIN)
    {
      throw microloop::KernelException(errno);
    }

    if (sent != -1)
    {
      total_sent += sent;
    }
  } while (total_sent != buf.size());

  return total_sent;
}

template <typename Callback>
static void send(int sock, const Buffer &buf, Callback on_sent)
{
  auto event_loop = microloop::EventLoop::get_main();
  auto &thread_pool = event_loop->get_thread_pool();

  auto wrapped_fn = [sock, &buf, on_sent]() {
    ssize_t sent = microloop::net::utils::send(sock, buf);
    on_sent(sent);
  };

  thread_pool.submit(wrapped_fn);
}

}  // namespace microloop::net::utils
