//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/rfc7230.h"

#include "absl/strings/str_split.h"
#include "utils/string.h"

#include <vector>

namespace microhttp::http
{

std::optional<StartLineData> RFC7230::parse_start_line(std::string_view str) noexcept
{
  auto l = str.size();
  if (l <= 2)
  {
    return std::nullopt;
  }

  if (!utils::string::ends_with(str, "\r\n"))
  {
    return std::nullopt;
  }

  str.remove_suffix(2);

  std::vector<std::string_view> parts = absl::StrSplit(str, ' ');

  /*
   * The start line is made of exactly THREE parts, per the standard: the HTTP method, the request
   * target, and the HTTP version.
   */
  if (parts.size() != 3)
  {
    return std::nullopt;
  }

  for (const auto part : parts)
  {
    if (part.empty())
    {
      return std::nullopt;
    }
  }

  auto [version, valid_version] = Version::from_string(parts[2]);
  if (!valid_version)
  {
    return std::nullopt;
  }

  return StartLineData{parts[0], parts[1], version};
}

std::optional<HeaderLineData> RFC7230::parse_header_line(std::string_view sv) noexcept
{
  auto l = sv.size();

  if (l <= 2 || !utils::string::ends_with(sv, "\r\n"))
  {
    return std::nullopt;
  }

  sv.remove_suffix(2);  // remove the CRLF token

  std::vector<std::string_view> parts = absl::StrSplit(sv, ':');
  if (parts.size() != 2)
  {
    return std::nullopt;  // does not respect the "Name: Value" format
  }

  if (utils::string::starts_with(parts[0], " ") || utils::string::ends_with(parts[0], " "))
  {
    return std::nullopt;
  }

  auto value = parts[1];

  std::size_t value_start = 0;
  while (value_start < value.size() && (value[value_start] == ' ' || value[value_start] == '\t'))
  {
    value_start++;
  }

  std::size_t value_end = value.size() - 1;
  while (value_end > value_start && (value[value_end] == ' ' || value[value_end] == '\t'))
  {
    value_end--;
  }

  value.remove_prefix(value_start);  // remove the leading whitespace
  value.remove_suffix(value_end);  // remove the trailing whitespace

  return HeaderLineData{parts[0], value};
}

}  // namespace microhttp::http
