//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/request_parser.h"

#include "microhttp/version.h"

#include <algorithm>
#include <string_view>

namespace microhttp::http
{

const std::size_t RequestParser::MAX_REQUEST_LINE_LEN = 2048;

RequestParser::RequestParser() : status{Status::NO_DATA}, expected_line_type{START_LINE}
{}

void RequestParser::add_chunk(const microloop::Buffer &buf)
{
  if (buf.empty())
  {
    return;
  }

  request_buffer.concat(buf);

  auto sv = request_buffer.str_view();
  std::size_t crlf_pos = 0;

  while ((crlf_pos = sv.find("\r\n", 0)) != std::string_view::npos)
  {
    parse_line(sv.substr(0, crlf_pos + 2));
    sv.remove_prefix(crlf_pos + 2);
  }
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

bool RequestParser::parse_start_line(std::string_view str, HttpRequest &req)
{
  if (str.size() < 2 || str[str.size() - 2] != '\r' || str[str.size() - 1] != '\n')
  {
    /*
     * the line does not end with CRLF token
     */
    return false;
  }

  std::size_t method_end_idx = str.find_first_of(' ');
  if (method_end_idx == std::string::npos || method_end_idx == 0)
  {
    /*
     * method string does not exist
     */
    return false;
  }

  req.set_http_method(std::string{str.substr(0, method_end_idx)});

  std::size_t uri_end_idx = str.find_first_of(' ', method_end_idx + 1);
  if (uri_end_idx == std::string::npos)
  {
    return false;
  }

  auto uri = str.substr(method_end_idx + 1, uri_end_idx - method_end_idx - 1);
  if (uri.empty())
  {
    return false;
  }

  req.set_uri(std::string{uri});

  if (!req.set_http_version(std::string{str.substr(uri_end_idx + 1, Version::STRING_LEN)}))
  {
    /*
     * version string is invalid
     */
    return false;
  }

  return true;
}

bool RequestParser::parse_header_line(std::string_view str, HttpRequest &req)
{
  if (str.size() < 2 || str[str.size() - 2] != '\r' || str[str.size() - 1] != '\n')
  {
    /*
     * the line does not end with CRLF token
     */
    return false;
  }

  auto colon_idx = str.find(':');
  if (colon_idx == std::string_view::npos)
  {
    /*
     * we do not have a field name/value separator
     */
    return false;
  }

  auto space_idx = str.find(' ');
  if (space_idx != std::string_view::npos && space_idx < colon_idx)
  {
    /*
     * whitespace not allowed before the colon
     */
    return false;
  }

  auto i = str.find_first_not_of(" \t", colon_idx + 1); /* start index of the field value */
  auto j = str.size() - 1;                              /* the end index of the field value */
  while (str[j] == '\r' || str[j] == '\n' || str[j] == ' ' || str[j] == '\t')
  {
    j--;
  }

  auto field_name = std::string{str.substr(0, colon_idx)};
  auto field_value = std::string{str.substr(i, j - i + 1)};

  req.set_header(field_name, field_value);

  return true;
}

void RequestParser::parse_line(std::string_view curr_line)
{
  switch (expected_line_type)
  {
  case START_LINE:
    if (!parse_start_line(curr_line, request))
    {
      status = INVALID_START_LINE;
    }
    else
    {
      expected_line_type = HEADER;
      status = NO_HEADERS;
    }
    break;

  case HEADER:
    if (!parse_header_line(curr_line, request))
    {
      status = INVALID_HEADER_LINE;
    }
    else
    {
      expected_line_type = HEADER_OR_CRLF;
      status = OK;
    }
    break;

  case HEADER_OR_CRLF:
    if (curr_line == "\r\n")
    {
      expected_line_type = BODY;
      status = OK;
    }
    else if (!parse_header_line(curr_line, request))
    {
      status = INVALID_HEADER_LINE;
    }
    break;
  case BODY:
    /*
     * TODO append to the request bdoy buffer
     */
    break;
  case END:
    break;
  }
}

}  // namespace microhttp::http
