//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microhttp/http_request.h"
#include "microhttp/rfc7230.h"
#include "microloop/buffer.h"

#include <string>

namespace microhttp::http
{

class BasicRequestParser
{
public:
  enum State
  {
    WAITING_START_LINE,
    WAITING_HEADER_LINE,
    WAITING_BODY,
    BODY,
    DONE,
    ERROR,
  };

  static constexpr std::size_t MAX_REQUEST_LINE_LEN = 2048;
};

class RequestParser : protected RFC7230, public BasicRequestParser
{
public:
  RequestParser() : state{State::WAITING_START_LINE}
  {}

  /**
   * Add a new chunk to be parsed by the parser.
   * @param buf The buffer to append to the existing one.
   */
  void add_chunk(const microloop::Buffer &buf);

  /**
   * Retrieve the current state of the parser. If, at any interogation, this function returns
   * an error indicating state, then the caller should reject the request.
   */
  State get_state() const;

  /**
   * Retrieve the HTTP request as parsed so far.
   */
  HttpRequest &get_parsed_request();

  /**
   * Retrieve the HTTP request as parsed so far.
   */
  const HttpRequest &get_parsed_request() const;

  /**
   * \brief Reset the parser.
   */
  void reset();

private:
  /**
   * The request as parsed so far by the parser.
   */
  HttpRequest request;

  /**
   * The buffer to be consumed and parsed.
   */
  microloop::Buffer next_unit;

  /**
   * The current state of the parser. If an error-indicating stastus is set, parsing should stop.
   */
  State state;
};

}  // namespace microhttp::http
