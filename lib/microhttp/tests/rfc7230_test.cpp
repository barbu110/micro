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

}  // namespace microhttp::http
