//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/request_parser.h"

#include "microhttp/version.h"

#include <algorithm>
#include <string_view>
#include <iostream>

namespace microhttp::http
{

const std::size_t BasicRequestParser::MAX_REQUEST_LINE_LEN = 2048;

void RequestParser::add_chunk(const microloop::Buffer &buf)
{

}

RequestParser::Status RequestParser::get_status() const
{
  return status;
}

const HttpRequest &RequestParser::get_parsed_request() const
{
  return request;
}

HttpRequest &RequestParser::get_parsed_request()
{
  return request;
}

}  // namespace microhttp::http
