//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/version.h"

#include <cctype>
#include <sstream>

namespace microhttp::http
{

const std::string Version::HEADER = "HTTP/";

std::pair<Version, bool> Version::from_string(std::string_view sv)
{
  Version v{0, 0};

  if (sv.size() <= HEADER.size() || sv.substr(0, HEADER.size()) != HEADER)
  {
    return std::make_pair(v, false);
  }

  if (!std::isdigit(sv[5]) || !std::isdigit(sv[7]) || sv[6] != '.')
  {
    return std::make_pair(v, false);
  }

  v.major = sv[5] - '0';
  v.minor = sv[7] - '0';

  if (v.major == 0)
  {
    return std::make_pair(v, false);
  }

  return std::make_pair(v, true);
}

Version::operator std::string() const
{
  std::ostringstream ss;
  ss << HEADER;
  ss << static_cast<std::uint32_t>(major) << "." << static_cast<std::uint32_t>(minor);

  return ss.str();
}

bool operator==(const Version &a, const Version &b)
{
  return a.major == b.major && a.minor == b.minor;
}

}  // namespace microhttp::http
