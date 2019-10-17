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

  if (parts.front().empty())
  {
    return std::nullopt;  // cannot allow name to be empty
  }

  if (utils::string::starts_with(parts[0], " ") || utils::string::ends_with(parts[0], " "))
  {
    return std::nullopt;
  }

  auto &value = parts[1];

  if (auto first_nonws_idx = value.find_first_not_of(" \t"); first_nonws_idx == 0)
  {
    return std::nullopt;  // value of the header does not start with whitespace
  }
  else
  {
    value.remove_prefix(std::min(first_nonws_idx, value.size()));
  }

  if (auto last_nonws_idx = value.find_last_not_of(" \t"); last_nonws_idx != std::string_view::npos)
  {
    value.remove_suffix(value.size() - last_nonws_idx - 1);
  }

  return HeaderLineData{parts[0], value};
}

}  // namespace microhttp::http
