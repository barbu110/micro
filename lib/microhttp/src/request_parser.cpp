//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/request_parser.h"

#include "microhttp/constants.h"
#include "microhttp/version.h"

#include <algorithm>
#include <string_view>

namespace microhttp::http
{

void RequestParser::add_chunk(const microloop::Buffer &buf)
{
  next_unit.concat(buf);

  while (!next_unit.empty())
  {
    if (state == ERROR)
    {
      /*
       * This request cannot be salvaged from the error state.
       */

      return;
    }

    if (state == WAITING_BODY)
    {
      if (!request.get_content_length())
      {
        return;
      }

      auto remaining_body_size = *request.get_content_length() - request.get_body().size();

      request.get_body().concat(next_unit, remaining_body_size);
      next_unit.clear();
      return;
    }

    auto crlf_idx = next_unit.str_view().find(constants::crlf);
    if (crlf_idx == std::string_view::npos)
    {
      /*
       * We received a request fragment that we cannot parse in any way. Return and wait for
       * another.
       */
      return;
    }

    auto line = next_unit.str_view(0, crlf_idx + constants::crlf_size);

    switch (state)
    {
    case WAITING_START_LINE:
    {
      auto start_line = parse_start_line(line);
      if (!start_line)
      {
        state = ERROR;
        break;
      }

      request.set_http_method(std::string{start_line->method});
      request.set_uri(std::string{start_line->request_target});
      request.set_http_version(start_line->version);

      state = WAITING_HEADER_LINE;
      break;
    }
    case WAITING_HEADER_LINE:
    {
      if (crlf_idx == 0)
      {
        state = WAITING_BODY;
        break;
      }

      auto header_line = parse_header_line(line);
      if (!header_line)
      {
        state = ERROR;
        break;
      }

      request.set_header(std::string{header_line->name}, std::string{header_line->value});

      // The state is not changed intentionally.

      break;
    }
    default:
      __builtin_unreachable();
    }

    next_unit.remove_prefix(crlf_idx + constants::crlf_size);
  }
}

RequestParser::State RequestParser::get_state() const
{
  return state;
}

const HttpRequest &RequestParser::get_parsed_request() const
{
  return request;
}

HttpRequest &RequestParser::get_parsed_request()
{
  return request;
}

void RequestParser::reset()
{
  state = WAITING_START_LINE;
  request = HttpRequest{};
  next_unit.clear();
}

}  // namespace microhttp::http
