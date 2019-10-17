//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/rfc7230.h"

#include "gtest/gtest.h"
#include <optional>
#include <string>
#include <string_view>
#include <tuple>

namespace microhttp::http
{

using StartLineFixture = std::tuple<const char *, std::optional<StartLineData>>;

class StartLineTest : public RFC7230, public testing::TestWithParam<StartLineFixture>
{};

TEST_P(StartLineTest, Works)
{
  auto [buf, expected_info] = GetParam();
  std::string_view start_line{buf};

  auto actual = parse_start_line(start_line);

  if (!expected_info.has_value())
  {
    ASSERT_FALSE(actual.has_value());
  }
  else
  {
    ASSERT_TRUE(actual.has_value());

    EXPECT_EQ(actual->method, expected_info->method);
    EXPECT_EQ(actual->request_target, expected_info->request_target);
    EXPECT_EQ(actual->version, expected_info->version);
  }
}

using HeaderLineFixture = std::tuple<const char *, std::optional<HeaderLineData>>;

class HeaderLineTest : public RFC7230, public testing::TestWithParam<HeaderLineFixture>
{};

TEST_P(HeaderLineTest, Works)
{
  auto [buf, expected_info] = GetParam();
  std::string_view header_line{buf};

  auto actual = parse_header_line(header_line);

  if (!expected_info.has_value())
  {
    ASSERT_FALSE(actual.has_value());
  }
  else
  {
    ASSERT_TRUE(actual.has_value());

    ASSERT_EQ(actual->name, expected_info->name);
    ASSERT_EQ(actual->value, expected_info->value);
  }
}

INSTANTIATE_TEST_CASE_P(RFC7230, StartLineTest,
    testing::ValuesIn(std::vector<StartLineFixture>{
        std::make_tuple("GET / HTTP/1.1\r\n", StartLineData{"GET", "/", Version{1, 1}}),  // valid
        std::make_tuple("GET / HTTP/1.1", std::nullopt),  // no CRLF
        std::make_tuple("GET / HTTP/v1.1\r\n", std::nullopt),  // invalid HTTP version
        std::make_tuple("GET / \r\n", std::nullopt),  // no HTTP verwsion
        std::make_tuple("GET HTTP/1.1\r\n", std::nullopt),  // no request target
        std::make_tuple("/ HTTP/1.1\r\n", std::nullopt),  // no method
        std::make_tuple("HTTP/1.1\r\n", std::nullopt),  // missing method and request target
        std::make_tuple("", std::nullopt),  // obvious?
        std::make_tuple(" GET / HTTP/1.1\r\n", std::nullopt),  // leading whitespace
        std::make_tuple("GET / HTTP/1.1 \r\n", std::nullopt)  // trailing whitespace
    }));

INSTANTIATE_TEST_CASE_P(RFC7230, HeaderLineTest,
    testing::ValuesIn(std::vector<HeaderLineFixture>{
        std::make_tuple("name: value\r\n", HeaderLineData{"name", "value"}),  // valid
        std::make_tuple("Header-Name: some value\r\n", HeaderLineData{"Header-Name", "some value"}),
        std::make_tuple("Name:     value   \r\n", HeaderLineData{"Name", "value"}),
        std::make_tuple("name: \r\n", HeaderLineData{"name", ""}),  // empty value
        std::make_tuple("name:\r\n", HeaderLineData{"name", ""}),  // also empty value
        std::make_tuple("Name: value", std::nullopt),  // no CRLF
        std::make_tuple("Name:value\r\n", std::nullopt),  // no space after semicolon
        std::make_tuple(" name: value\r\n", std::nullopt),  // leading space; unallowed
        std::make_tuple("name : value\r\n", std::nullopt),  // space before semicolon
        std::make_tuple("name. value\r\n", std::nullopt),  // no semicolon, so no value
        std::make_tuple(": value\r\n", std::nullopt),  // no header name before semicolon
        std::make_tuple(":\r\n", std::nullopt),  // empty header name and value
        std::make_tuple("", std::nullopt),  // empty string
        std::make_tuple("\r\n", std::nullopt),  // empty line
    }));

}  // namespace microhttp::http
