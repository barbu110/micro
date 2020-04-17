//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/http_request.h"
#include "microhttp/version.h"

#include "gtest/gtest.h"

namespace microhttp::http
{

TEST(HttpRequest, DefaultConstructor)
{
  HttpRequest req;

  EXPECT_EQ(req.get_http_version().major, 1);
  EXPECT_EQ(req.get_http_version().minor, 1);
  EXPECT_TRUE(req.get_headers().empty());
}

TEST(HttpRequest, SetHttpMethod)
{
  HttpRequest req;

  req.set_http_method("GET");

  EXPECT_EQ(req.get_http_method(), "get");
}

TEST(HttpRequest, SetHeader)
{
  HttpRequest req;

  req.set_header("content-type", "text/plain");

  auto [header_value, found] = req.get_header("content-type");
  EXPECT_TRUE(found);
  EXPECT_EQ(header_value, "text/plain");
}

TEST(HttpRequest, GetContentLength)
{
  HttpRequest req;

  ASSERT_FALSE(req.get_content_length().has_value());

  req.set_header("Content-Length", "128");

  auto content_length = req.get_content_length();
  ASSERT_TRUE(content_length.has_value());
  EXPECT_EQ(128, *content_length);
}

}  // namespace microhttp::http
