//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microhttp/http_request.h"
#include "microloop/buffer.h"

#ifdef BUILD_TESTS
#include "gtest/gtest_prod.h"
#endif

#include <string>

namespace microhttp::http
{

class RequestParser
{
#ifdef BUILD_TESTS
  FRIEND_TEST(RequestParser, ParseStartLine);
  FRIEND_TEST(RequestParser, ParseHeaderLine);
  FRIEND_TEST(RequestParser, AddChunkValid);
#endif

  enum ExpectedLine
  {
    END,
    START_LINE,
    HEADER,
    HEADER_OR_CRLF,
    BODY,
  };

public:
  static const std::size_t MAX_REQUEST_LINE_LEN;

  enum Status
  {
    NO_DATA,
    NO_HEADERS,
    OK,
    INVALID_START_LINE,
    INVALID_HEADER_LINE,
    INVALID_CONTENT_LENGTH,
    FINISHED,
  };

  RequestParser();

  /**
   * Add a new chunk to be parsed by the parser.
   * @param buf The buffer to append to the existing one.
   */
  void add_chunk(const microloop::Buffer &buf);

  /**
   * Retrieve the current status of the parser. If, at any interogation, this function returns
   * an error indicating status, then the caller should reject the request.
   */
  Status get_status() const;

  /**
   * Retrieve the HTTP request as parsed so far.
   */
  HttpRequest &get_parsed_request();

  /**
   * Retrieve the HTTP request as parsed so far.
   */
  const HttpRequest &get_parsed_request() const;

private:
  /**
   * Parse the current line according to its expected type.
   */
  void parse_line(std::string_view sv);

  static bool parse_start_line(std::string_view sv, HttpRequest &req);
  static bool parse_header_line(std::string_view sv, HttpRequest &req);

private:
  /**
   * The request as parsed so far by the parser.
   */
  HttpRequest request;

  /**
   * The buffer to be consumed and parsed.
   */
  microloop::Buffer request_buffer;

  /**
   * The current status of the parser. If an error-indicating stastus is set, parsing should stop.
   */
  Status status;

  /**
   * The expected type of the current line to be parsed. If parsing will fail according to this
   * expected type, then an error-indicating status will be set.
   */
  ExpectedLine expected_line_type;
};

}  // namespace microhttp::http
