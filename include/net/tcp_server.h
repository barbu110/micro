//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <string>
#include <cstdint>

namespace microloop::utils {

class TcpServer {
public:
  TcpServer(std::string host, std::uint16_t port);
  virtual ~TcpServer();

  template <class Func>
  virtual void on_start(Func &&fn);

  template <class Func>
  virtual void on_connect(Func &&fn);

  template <class Func>
  virtual void on_request(Func &&fn);

  template <class Func>
  virtual void on_disconnect(Func &&fn);

  template <class Func>
  virtual void on_shutdown(Func &&fn);
};

}
