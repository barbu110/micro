//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <type_traits>
#include <string_view>

namespace utils::string
{

template <typename T>
inline constexpr bool is_c_string = std::is_same_v<T, char *> || std::is_same_v<T, const char *>;

template <typename T, typename U>
std::enable_if_t<!is_c_string<T> && !is_c_string<U>, bool> ends_with(T sv, U needle) noexcept
{
  if (needle.empty())
  {
    return true;
  }

  if (sv.size() < needle.size())
  {
    return false;
  }

  if (sv == needle)
  {
    return true;
  }

  typename T::size_type p = sv.rfind(needle);
  if (p == T::npos)
  {
    return false;
  }

  return sv.size() - p == needle.size();
}

template <typename T, typename U>
std::enable_if_t<is_c_string<T> || is_c_string<U>, bool> ends_with(T sv, U needle) noexcept
{
  return ends_with(std::string_view(sv), std::string_view(needle));
}

template <typename T, typename U>
std::enable_if_t<!is_c_string<T> && !is_c_string<U>, bool> starts_with(T s, U needle) noexcept
{
  if (needle.empty() || s == needle)
  {
    return true;
  }

  if (s.empty() || needle.size() > s.size())
  {
    return false;
  }

  return s.find(needle) == 0;
}

template <typename T, typename U>
std::enable_if_t<is_c_string<T> || is_c_string<U>, bool> starts_with(T s, U needle)
{
  return starts_with(std::string_view(s), std::string_view(needle));
}

}  // namespace utils::string
