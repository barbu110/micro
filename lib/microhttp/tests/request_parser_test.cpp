//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "gtest/gtest.h"

#include "microhttp/http_request.h"
#include "microhttp/request_parser.h"
#include "microhttp/version.h"

#include <string>
#include <vector>

namespace microhttp::http
{

struct ReqStartLineInfo
{
  std::string start_line;
  std::string method;
  std::string uri;
  Version http_version;
};

struct HeaderInfo
{
  std::string header_line;
  std::string field_name;
  std::string field_value;
};

TEST(RequestParser, ParseStartLine)
{
  std::vector<ReqStartLineInfo> valid_cases = {
      {"GET http://localhost/ HTTP/1.1\r\n", "get", "http://localhost/", Version{1, 1}},
      {"POST http://example.com HTTP/1.0\r\n", "post", "http://example.com", Version{1, 0}},
  };

  for (const auto &c : valid_cases)
  {
    HttpRequest req;
    auto valid = RequestParser::parse_start_line(c.start_line, req);

    ASSERT_TRUE(valid);
    ASSERT_EQ(req.get_http_method(), c.method);
    ASSERT_EQ(req.get_uri(), c.uri);
    ASSERT_EQ(req.get_http_version().major, c.http_version.major);
    ASSERT_EQ(req.get_http_version().minor, c.http_version.minor);
  }

  std::vector<std::string> invalid_start_lines = {
      "http://localhost\r\n",
      "http://localhost",
      "GET\r\n",
      "GET HTTP/1.1\r\n",
      "GET HTTP\r\n",
      "GET / HTTP\r\n",
  };

  for (const auto &c : invalid_start_lines)
  {
    HttpRequest req;
    ASSERT_FALSE(RequestParser::parse_start_line(c, req));
  }
}

TEST(RequestParser, ParseHeaderLine)
{
  std::vector<HeaderInfo> valid_header_lines = {
      {"Content-Type: application/json\r\n", "Content-Type", "application/json"},
      {"Content-Type:text/plain    \r\n", "Content-Type", "text/plain"},
      {"Content-Length:    17   \r\n", "Content-Length", "17"},
      {"x-powered-by:\tmicroloop v0.9.1   \r\n", "X-Powered-By", "microloop v0.9.1"},
  };

  for (const auto &c : valid_header_lines)
  {
    HttpRequest req;
    ASSERT_TRUE(RequestParser::parse_header_line(c.header_line, req));

    auto [field_value, found] = req.get_header(c.field_name);

    EXPECT_TRUE(found);
    EXPECT_EQ(field_value, c.field_value);
  }

  std::vector<std::string> invalid_header_lines = {
      "Content-Type : application/json\r\n",
      "Content Type: text/plain\r\n",
      "Content-Length = 17\r\n",
      " X-Powered-By: microloop\r\n",
  };

  for (const auto &c : invalid_header_lines)
  {
    HttpRequest req;
    ASSERT_FALSE(RequestParser::parse_header_line(c, req));
  }
}

}  // namespace microhttp::http
