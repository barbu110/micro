//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <atomic>
#include <buffer.h>
#include <cstdint>
#include <errno.h>
#include <event_source.h>
#include <kernel_exception.h>
#include <sys/socket.h>

namespace microloop::event_sources::net
{

template <bool oneshot = true>
class Receive : public microloop::EventSource, public microloop::TypeHelper<microloop::Buffer>
{
public:
  static const std::uint32_t DEFAULT_MAX_READ_SIZE = 4096;  // The default page size on many systems

  Receive(std::uint32_t sock, Callback &&on_recv,
      std::uint32_t max_read_size = Receive::DEFAULT_MAX_READ_SIZE) :
      EventSource{sock},
      on_recv{std::move(on_recv)},
      max_read_size{max_read_size}
  {}

  std::uint32_t produced_events() const override
  {
    return EPOLLIN;
  }

  bool native_async() const override
  {
    return false;
  }

  void start() override
  {
    if (oneshot)
    {
      run_recv();
    }
  }

  void run_callback() override
  {
    if (!oneshot)
    {
      run_recv();
    }

    std::apply(on_recv, get_return_object());
  }

private:
  void run_recv()
  {
    microloop::Buffer buf{max_read_size};
    ssize_t nrecv = recv(get_fd(), buf.data(), buf.size(), 0);
    if (nrecv == -1)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {
        return;
      }

      throw microloop::KernelException(errno);
    }

    buf.resize(nrecv);

    set_return_object(buf);
  }

private:
  Callback on_recv;
  const std::uint32_t max_read_size;
};

}  // namespace microloop::event_sources::net
