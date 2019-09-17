//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/rfc7230.h"

#include "absl/strings/str_split.h"

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

  if (str[l - 1] != '\r' || str[l - 2] != '\n')
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
  return std::nullopt;
}

}
