//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "utils/string.h"

#include "gtest/gtest.h"
#include <string>
#include <tuple>

namespace utils::string
{

class StartsWithTest : public testing::TestWithParam<std::tuple<std::string, std::string, bool>>
{};

TEST_P(StartsWithTest, WorksWithObjects)
{
  auto [sv, needle, expected] = GetParam();
  auto actual = starts_with(sv, needle);

  EXPECT_EQ(actual, expected);
}

TEST(StartsWithTest, WorksWithConstCString)
{
  const char *needle = "foo";
  const char *sv = "foo bar";

  EXPECT_TRUE(starts_with(sv, needle));
  EXPECT_TRUE(starts_with(std::string{sv}, needle));
  EXPECT_TRUE(starts_with(sv, std::string_view{needle}));
}

INSTANTIATE_TEST_CASE_P(Utils, StartsWithTest,
    testing::Values(std::make_tuple("", "", true), std::make_tuple("foo", "", true),
        std::make_tuple("foo", "foo", true), std::make_tuple("foo", "bar", false),
        std::make_tuple("foo", "f", true), std::make_tuple("foo", "longer", false)));

}  // namespace utils::string
