//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "gtest/gtest.h"

#include "microloop/buffer.h"

#include <string>
#include <cstring>
#include <cstdint>

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

TEST(Buffer, ConstructsFromCString)
{
  const char *constructor_string = "c_string";
  Buffer buf{constructor_string};

  const char *actual_data = static_cast<char *>(buf.data());

  ASSERT_EQ(buf.size(), std::strlen(actual_data) + 1);
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
  const char *str_a = "a";
  const char *str_b = "b";

  const char expected_result[] = {'a', '\0', 'b', '\0'};

  Buffer a{str_a}, b{str_b};

  a.concat(b);

  /*
   * 2 is the number of NULL-characters.
   */
  ASSERT_EQ(a.size(), std::strlen(str_a) + std::strlen(str_b) + 2);

  const char *actual = static_cast<const char *>(a.data());
  for (std::size_t i = 0; i < std::size(expected_result); i++)
  {
    ASSERT_EQ(actual[i], expected_result[i]);
  }
}

TEST(Buffer, ConcatsCorrectlyWithSize)
{
  const char *str_a = "a";
  const char *str_b = "b";

  const char expected_result[] = {'a', '\0', 'b'};

  Buffer a{str_a}, b{str_b};

  a.concat(b, 1);

  /*
   * The term 1 comes from the NULL charascter from str_a.
   */
  ASSERT_EQ(a.size(), std::strlen(str_a) + std::strlen(str_b) + 1);

  const char *actual = static_cast<const char *>(a.data());
  for (std::size_t i = 0; i < std::size(expected_result); i++)
  {
    ASSERT_EQ(actual[i], expected_result[i]);
  }
}

}
