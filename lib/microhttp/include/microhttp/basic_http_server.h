//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microhttp/http_response.h"
#include "microloop/net/tcp_server.h"

#include <cstdint>
#include <function>
#include <map>
#include <tuple>

namespace microhttp
{

class BasicHttpServer : protected microloop::net::TcpServer
{
  using microhttp::http::HttpRequest;
  using microhttp::http::HttpResponse;

public:
  using RequestHandler = std::function<void(const HttpRequest &, HttpResponse &)>;

  BasicHttpServer(std::uint16_t port = DEFAULT_HTTP_PORT);


private:
  std::uint16_t port_;
  std::map<std::string, std::tuple<std::string, RequestHandler>> mapping;
};

}  // namespace microhttp
