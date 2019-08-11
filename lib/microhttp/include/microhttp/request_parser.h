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
  FRIEND_TEST(RequestParser, AddChunk);
#endif

  enum ExpectedLine
  {
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
    OK,
    INVALID_START_LINE,
    INVALID_HEADER_LINE,
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
  HttpRequest get_parsed_request() const;

private:
  /**
   * Parse the current line according to its expected type.
   */
  void parse_line();

  static bool parse_start_line(const std::string &str, HttpRequest &req);
  static bool parse_header_line(const std::string &str, HttpRequest &req);

private:
  /**
   * The request as parsed so far by the parser.
   */
  HttpRequest request;

  /**
   * The buffer to be consumed and parsed.
   */
  microloop::Buffer buffer;

  /**
   * The current status of the parser. If an error-indicating stastus is set, parsing should stop.
   */
  Status status;

  /**
   * The current line to be parsed INCLUDING the CRLF token at the end.
   */
  std::string curr_line;

  /**
   * The expected type of the current line to be parsed. If parsing will fail according to this
   * expected type, then an error-indicating status will be set.
   */
  ExpectedLine expected_line_type;
};

}  // namespace microhttp::http
