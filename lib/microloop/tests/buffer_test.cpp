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
  Buffer buf;

  ASSERT_EQ(buf.data(), nullptr);
  ASSERT_EQ(buf.size(), 0);
}

TEST(Buffer, ConstructsWithSize)
{
  static std::size_t buf_size = 128;
  Buffer buf{buf_size};

  ASSERT_EQ(buf.size(), buf_size);

  char *data = static_cast<char *>(buf.data());
  for (std::size_t i = 0; i < buf.size(); i++)
  {
    ASSERT_EQ(data[i], 0);
  }
}

TEST(Buffer, ConstructsWithSizeZero)
{
  std::size_t zero = 0;
  Buffer buf(zero);

  ASSERT_EQ(buf.size(), 0);
  ASSERT_EQ(buf.data(), nullptr);

  Buffer cpy{buf};

  ASSERT_EQ(cpy.size(), 0);
  ASSERT_EQ(cpy.data(), nullptr);
}

TEST(Buffer, ConstructsFromCString)
{
  const char *constructor_string = "c_string";
  Buffer buf{constructor_string};

  const char *actual_data = static_cast<char *>(buf.data());

  ASSERT_EQ(buf.size(), std::strlen(actual_data));
  ASSERT_STREQ(actual_data, constructor_string);
}

TEST(Buffer, ConstructsFromTruncatedCString)
{
  const char *constructor_string = "c_string";
  Buffer buf{constructor_string, 2}; /* we want to construct with "c_" without null */

  ASSERT_EQ(buf.size(), 2);

  const char *data = static_cast<char *>(buf.data());
  ASSERT_EQ(data[0], 'c');
  ASSERT_EQ(data[1], '_');
}

TEST(Buffer, ConcatsCorrectlyWithoutSize)
{
  const char *str_a = "ab";
  const char *str_b = "cd";

  const char expected_result[] = {'a', 'b', 'c', 'd'};

  Buffer a{str_a}, b{str_b};

  a.concat(b);

  ASSERT_EQ(a.size(), std::strlen(str_a) + std::strlen(str_b));

  const char *actual = static_cast<const char *>(a.data());
  for (std::size_t i = 0; i < std::size(expected_result); i++)
  {
    ASSERT_EQ(actual[i], expected_result[i]);
  }
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

TEST(Buffer, RemovePrefix)
{
  microloop::Buffer buf{"foo bar"};

  EXPECT_STREQ("foo bar", buf.str().c_str());

  buf.remove_prefix(4);
  EXPECT_STREQ("bar", buf.str().c_str());
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
