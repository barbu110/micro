//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/buffer.h"

#include "gtest/gtest.h"
#include <cstdint>
#include <cstring>
#include <string>
#include <tuple>

namespace microloop
{

TEST(Buffer, DefaultConstructs)
{
  Buffer a;

  ASSERT_EQ(a.data(), nullptr);
  ASSERT_EQ(a.size(), 0);
}

TEST(Buffer, ConstructsWithSize)
{
  static constexpr std::size_t test_size = 128;
  Buffer b{test_size};
  std::uint8_t *data = static_cast<std::uint8_t *>(b.data());

  ASSERT_EQ(b.size(), test_size);

  for (std::size_t i = 0; i != test_size; i++)
  {
    EXPECT_EQ(data[i], 0);
  }
}

TEST(Buffer, ConstructsFromCString)
{
  const char *constructor_string = "c_string";
  Buffer buf{constructor_string};

  ASSERT_EQ(buf.size(), std::strlen(constructor_string));
  ASSERT_EQ(std::memcmp(buf.data(), constructor_string, std::strlen(constructor_string)), 0);

  Buffer cpy{buf};

  ASSERT_EQ(cpy.size(), buf.size());
  ASSERT_EQ(std::memcmp(cpy.data(), buf.data(), buf.size()), 0);
}

TEST(Buffer, ConstructsFromTruncatedCString)
{
  const char *constructor_string = "c_string";
  Buffer buf{constructor_string, 2}; /* we want to construct with "c_" without null */

  ASSERT_EQ(buf.size(), 2);
  ASSERT_EQ(std::memcmp(buf.data(), constructor_string, buf.size()), 0);
}

TEST(Buffer, ConcatsCorrectlyWithoutSize)
{
  Buffer a{"foo"}, b{"bar"};

  ASSERT_EQ(a.str_view(), "foo");
  ASSERT_EQ(b.str_view(), "bar");

  a.concat(b);

  ASSERT_EQ(a.size(), 6); /* the total length of "foo" and "bar" */
  ASSERT_EQ(std::memcmp(a.data(), "foobar", 6), 0);
}

TEST(Buffer, ConcatsCorrectlyWithSize)
{
  const char *str_a = "ab";
  const char *str_b = "cd";

  const char expected_result[] = {'a', 'b', 'c'};

  Buffer a{str_a}, b{str_b};

  constexpr std::size_t concat_size = 1;
  a.concat(b, concat_size);

  ASSERT_EQ(a.size(), std::strlen(str_a) + concat_size);

  const char *actual = static_cast<const char *>(a.data());
  for (std::size_t i = 0; i < std::size(expected_result); i++)
  {
    ASSERT_EQ(actual[i], expected_result[i]);
  }
}

TEST(Buffer, ConcatOperator)
{
  Buffer a{"foo"};
  Buffer b{1};
  Buffer c{"bar"};

  a += b;
  a += c;

  const char expected_data[] = {'f', 'o', 'o', '\0', 'b', 'a', 'r'};
  ASSERT_EQ(a.size(), sizeof(expected_data));
  ASSERT_EQ(std::memcmp(a.data(), expected_data, a.size()), 0);
}

TEST(Buffer, RemovePrefix)
{
  microloop::Buffer buf{"foo bar"};

  EXPECT_EQ(buf.str_view(), "foo bar");

  buf.remove_prefix(4);
  EXPECT_EQ(buf.str_view(), "bar");
}

TEST(Buffer, RemoveSuffix)
{
  microloop::Buffer buf{"foo bar"};

  EXPECT_STREQ("foo bar", buf.str().c_str());

  buf.remove_suffix(4);
  EXPECT_STREQ("foo", buf.str().c_str());
}

TEST(Buffer, Comparison)
{
  std::vector<std::tuple<microloop::Buffer, microloop::Buffer, bool>> cases = {
    std::make_tuple("", "", true),
    std::make_tuple("a", "a", true),
    std::make_tuple("", "a", false),
    std::make_tuple("a", "b", false),
  };

  for (const auto &t : cases)
  {
    auto &[a, b, is_equal] = t;

    ASSERT_EQ(a == b, is_equal);
  }
}

TEST(Buffer, Str)
{
  microloop::Buffer buf{"foo bar baz"};

  EXPECT_EQ(buf.str(), "foo bar baz");
  EXPECT_EQ(buf.str(0), "foo bar baz");
  EXPECT_EQ(buf.str(4), "bar baz");
  EXPECT_EQ(buf.str(0, 3), "foo");
  EXPECT_EQ(buf.str(4, 3), "bar");
  EXPECT_EQ(buf.str(0, 128), "foo bar baz");
}

TEST(Buffer, StrView)
{
  microloop::Buffer buf{"foo bar baz"};

  EXPECT_EQ(buf.str_view(), std::string_view{"foo bar baz"});
  EXPECT_EQ(buf.str_view(0), std::string_view{"foo bar baz"});
  EXPECT_EQ(buf.str_view(4), std::string_view{"bar baz"});
  EXPECT_EQ(buf.str_view(0, 3), std::string_view{"foo"});
  EXPECT_EQ(buf.str_view(4, 3), std::string_view{"bar"});
  EXPECT_EQ(buf.str_view(0, 128), std::string_view{"foo bar baz"});
}

}  // namespace microloop
