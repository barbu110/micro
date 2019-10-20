//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "gtest/gtest.h"

#include "microhttp/http_request.h"
#include "microhttp/request_parser.h"
#include "microhttp/version.h"
#include "microloop/buffer.h"

#include <string>
#include <vector>
#include <utility>
#include <ostream>

namespace microhttp::http
{

struct RequestTestProvider
{
  std::string note;
  std::vector<microloop::Buffer> chunks;
  bool valid;
  HttpRequest expected_request;
};

std::ostream &operator<<(std::ostream &os, const RequestTestProvider &test)
{
  os << test.note;
  return os;
}

std::vector<RequestTestProvider> request_parser_provider()
{
  std::vector<RequestTestProvider> tests;

  {
    std::vector<microloop::Buffer> chunks = {
      "GET / HTTP/1.1\r\n",
    };

    HttpRequest expected_request{"get", "/"};

    RequestTestProvider test{"valid simple GET request", chunks, true, expected_request};
    tests.push_back(test);
  }

  {
    std::vector<microloop::Buffer> chunks = {
      "GET / ",
      "HTTP/1.1\r\n",
    };

    HttpRequest expected_request{"get", "/"};

    RequestTestProvider test{"chunked GET request", chunks, true, expected_request};
    tests.push_back(test);
  }

  {
    std::vector<microloop::Buffer> chunks = {
      "POST /foo HTTP/1.1\r\n",
      "Content-Length: 0\r\n\r\n",
    };

    HttpRequest expected_request{"post", "/foo"};
    expected_request.set_header("Content-Length", "0");

    RequestTestProvider test{"chunked POST request, no body", chunks, true, expected_request};
    tests.push_back(test);
  }

  {
    std::vector<microloop::Buffer> chunks = {
      "POST /foo HTTP/1.1\r\n",
      "Content-Length: 7\r\n\r\n",
      "example",
    };

    HttpRequest expected_request{"post", "/foo"};
    expected_request.set_header("Content-Length", "7");
    expected_request.get_body() = microloop::Buffer{"example"}

    RequestTestProvider test{"chunked POST request, with body", chunks, true, expected_request};
    tests.push_back(test);
  }

  {
    std::vector<microloop::Buffer> chunks = {
      "POST /foo HTTP/1.1\r\n",
      "Content-Length: 7\r\n",
      "\r\nexample", /* note the CRLF before the body is in the same chunk with the body itself */
    };

    HttpRequest expected_request{"post", "/foo"};
    expected_request.set_header("Content-Length", "7");
    expected_request.get_body().concat("example");

    RequestTestProvider test{"chunked POST request, with body", chunks, true, expected_request};
    tests.push_back(test);
  }

  return tests;
}

class RequestParserTest : public testing::TestWithParam<RequestTestProvider>
{};

TEST_P(RequestParserTest, ParseRequest)
{
  auto &test = GetParam();

  RequestParser parser;

  auto &actual_request = parser.get_parsed_request();
  auto &expected_request = test.expected_request;

  for (const auto &c : test.chunks)
  {
    parser.add_chunk(c);
  }

  if (test.valid)
  {
    ASSERT_EQ(actual_request.get_http_version(), expected_request.get_http_version());
    ASSERT_EQ(actual_request.get_uri(), expected_request.get_uri());
    ASSERT_EQ(actual_request.get_http_method(), expected_request.get_http_method());
    ASSERT_EQ(actual_request.get_headers(), expected_request.get_headers());
    ASSERT_EQ(actual_request.get_body(), expected_request.get_body());
  }
}

INSTANTIATE_TEST_CASE_P(
  Http,
  RequestParserTest,
  testing::ValuesIn(request_parser_provider())
);

}
