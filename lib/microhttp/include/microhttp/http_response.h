//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

namespace microhttp::http
{

class HttpResponse
{
public:
  HttpResponse(const microloop::Buffer &content, std::uint16_t status_code = StatusCode::OK);

private:
  microhttp::http::Version http_version_;
  std::uint16_t status_code_;
  std::map<std::string, std::string> headers_;
  microloop::Buffer content_;
};

}
