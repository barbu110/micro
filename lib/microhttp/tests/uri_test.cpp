//
// Copyright (c) 2020 by Victor Barbu. All Rights Reserved.
//

#include "gtest/gtest.h"

#include "microhttp/uri.h"

#include <string>
#include <vector>
#include <tuple>

namespace microhttp::http
{

struct ValidUriParserTestProvider
{
  std::string description;
  std::string uri;
  Uri parsed;
};

std::ostream &operator<<(std::ostream &os, const ValidUriParserTestProvider &test)
{
  os << test.description;
  return os;
}

class ValidUriParserTest : public ::testing::TestWithParam<ValidUriParserTestProvider>
{};

TEST_P(ValidUriParserTest, Parse)
{
  const auto &test = GetParam();
}

INSTANTIATE_TEST_CASE_P(
  UriParser,
  ValidUriParserTest,
  testing::ValuesIn(std::vector<ValidUriParserTest>{
    std::make_tuple("complete URI",
        "scheme://user:pass@host:81/path?query#fragment",
        Uri{"scheme", "user:pass", "host", 81, "/path", "query", "fragment"}),
    std::make_tuple("URI is not normalized",
        "ScheMe://user:pass@HoSt:81/path?query#fragment",
        Uri{"ScheMe", "user:pass", "HoSt", 81, "path", "query", "fragment"}),
    std::make_tuple("URI without scheme",
        "//user:pass@HoSt:81/path?query#fragment",
        Uri{"", "user:pass", "HoSt", 81, "/path", "query", "fragment"}),
    std::make_tuple("only authority",
        "//",
        Uri{"", "", "", 0, "", "", ""}),
    std::make_tuple("without user info",
        "scheme://HoSt:81/path?query#fragment",
        Uri{"scheme", "", "HoSt", 81, "/path", "query", "fragment"}),
    std::make_tuple("empty user info",
        "scheme://@example.com:81/path?query#fragment",
        Uri{"scheme", "", "example.com", 81, "/path", "query", "fragment"}),
    std::make_tuple("without port",
        "scheme://user:pass@host/path?query#fragment",
        Uri{"scheme", "user:pass", "host", 0, "/path", "query", "fragment"}),
    std::make_tuple("with empty port",
        "scheme://user:pass@host:/path?query#fragment",
        Uri{"scheme", "user:pass", "host", 0, "/path", "query", "fragment"}),
    std::make_tuple("without user info and port",
        "scheme://host/path?query#fragment",
        Uri{"scheme", "", "host", 0, "/path", "query", "fragment"}),
    std::make_tuple("with IPv4 host",
        "scheme://10.0.0.2/p?q#f",
        Uri{"scheme", "", "10.0.0.2", 0, "/p", "q", "f"}),,
    std::make_tuple("with scoped IP",
        "scheme://[fe80:1234::%251]/p?q#f",
        Uri{"scheme", "", "[fe80:1234::%251]", 0, "/p", "q", "f"}),
    std::make_tuple("with IPvFuture",
        "scheme://[vAF.1::2::3]:8080/p?q#f",
        Uri{"scheme", "", "[vAF.1::2::3]", 8080, "/p","q", "f"}),
    std::make_tuple("without authority",
        "scheme:path?query#fragment",
        Uri{"scheme", "", "", 0, "path", "query", "fragment"}),
  })
);

}
