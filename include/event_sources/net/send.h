//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <atomic>
#include <buffer.h>
#include <event_source.h>
#include <functional>
#include <kernel_exception.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <tuple>
#include <unistd.h>

namespace microloop::event_sources::net
{

class Send : public microloop::EventSource
{
  using Types = microloop::TypeHelper<ssize_t>;

public:
  // The socket should be opened in nonblocking mode. We do not issue an fcntl to set
  // it that way because that would waste time.
  Send(std::uint32_t sock, const microloop::Buffer &buf, Types::Callback callback) :
      microloop::EventSource{sock}, buf{buf}, callback{callback}, sent_total{0}
  {}

  void start() override
  {
    auto buf_data = reinterpret_cast<std::uint8_t *>(buf.data());
    ssize_t sent = send(get_fd(), buf_data + sent_total, buf.size() - sent_total, 0);
    if (sent == -1 && errno != EWOULDBLOCK && errno != EAGAIN)
    {
      throw microloop::KernelException(errno);
    }

    if (sent != -1)
    {
      sent_total += sent;
    }

    mark_complete();  // FIXME If EWOULDBLOCK is returned, do not mark as complete.
    return_object = std::make_tuple(sent_total.load());
  }

  std::uint32_t produced_events() const override
  {
    return 0;
  }

  bool native_async() const override
  {
    return false;
  }

  bool needs_retry() const override
  {
    return sent_total != buf.size();
  }

  void run_callback() override
  {
    std::apply(callback, return_object);
  }

private:
  microloop::Buffer buf;
  Types::Callback callback;
  Types::ReturnType return_object;

  /**
   * The number of bytes sent in total during the life of this Send instance. This is used to tell
   * the event loop it needs to restart the operation
   */
  std::atomic<ssize_t> sent_total;
};

}  // namespace microloop::event_sources::net
