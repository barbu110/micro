//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "http/version.h"

#include <map>
#include <string>
#include <vector>

namespace microhttp::http
{

TEST(HttpVersion, FromString)
{
  using microhttp::http::Version;

  std::map<std::string, Version> valid_versions = {
      {"HTTP/1.0", Version{1, 0}},
      {"HTTP/1.1", Version{1, 1}},
  };

  for (const auto &p : valid_versions)
  {
    auto [version, valid] = Version::from_string(p.first);

    ASSERT_TRUE(valid);
    ASSERT_EQ(p.second.major, version.major);
    ASSERT_EQ(p.second.minor, version.minor);
  }

  std::vector<std::string> invalid_versions = {
      "",
      " ",
      "HTTP",
      "HTTP/",
      "HTTP/1",
      "HTTP/0.1",
      "HTTP/01",
      "HTTP.1",
      "HTTP/10",
      "HTTP/10.0",
  };

  for (const auto &p : invalid_versions)
  {
    auto [version, valid] = Version::from_string(p);

    ASSERT_FALSE(valid);
  }
}

TEST(HttpVersion, ToString)
{
  using microhttp::http::Version;

  std::map<std::string, Version> versions = {
      {"HTTP/1.0", Version{1, 0}},
      {"HTTP/1.1", Version{1, 1}},
  };

  for (const auto &p : versions)
  {
    auto version_string = static_cast<std::string>(p.second);

    EXPECT_EQ(p.first, version_string);
  }
}

}  // namespace microhttp::http
