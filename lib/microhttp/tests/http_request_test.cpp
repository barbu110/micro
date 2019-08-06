//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "gtest/gtest.h"

#include "microhttp/http_request.h"
#include "microhttp/version.h"

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

}
