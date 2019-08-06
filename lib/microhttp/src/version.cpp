//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/version.h"

#include <cctype>
#include <iostream>
#include <sstream>

namespace microhttp::http
{

const std::string Version::HEADER = "HTTP/";

std::pair<Version, bool> Version::from_string(const std::string &str)
{
  Version v{0, 0};

  if (str.size() <= HEADER.size() || str.substr(0, HEADER.size()) != HEADER)
  {
    return std::make_pair(v, false);
  }

  if (!std::isdigit(str[5]) || !std::isdigit(str[7]) || str[6] != '.')
  {
    return std::make_pair(v, false);
  }

  v.major = str[5] - '0';
  v.minor = str[7] - '0';

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

}  // namespace microhttp::http
