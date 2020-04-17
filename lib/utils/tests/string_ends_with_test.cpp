//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "utils/string.h"

#include "gtest/gtest.h"
#include <string>
#include <tuple>

namespace utils::string
{

class EndsWithTest : public testing::TestWithParam<std::tuple<std::string, std::string, bool>>
{};

TEST_P(EndsWithTest, Works)
{
  auto [sv, needle, expected] = GetParam();
  auto actual = ends_with(sv, needle);

  EXPECT_EQ(actual, expected);
}

TEST(EndsWithTest, WorksWithConstCString)
{
  const char *str = "foo";
  const char *needle = "o";

  EXPECT_TRUE(ends_with(str, needle));
}

TEST(EndsWithTest, WorksWithCString)
{
  char *str = "foo";
  char *needle = "o";

  EXPECT_TRUE(ends_with(str, needle));
}

INSTANTIATE_TEST_CASE_P(Utils, EndsWithTest,
    testing::Values(std::make_tuple("", "", true), std::make_tuple("foo", "", true),
        std::make_tuple("foo", "o", true), std::make_tuple("foo", "oo", true),
        std::make_tuple("foo", "foo", true), std::make_tuple("foo", "longer", false),
        std::make_tuple("foo", "bar", false), std::make_tuple("foo bar", "foo", false)));

}  // namespace utils::string
