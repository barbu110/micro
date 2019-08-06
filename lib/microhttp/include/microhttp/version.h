//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace microhttp::http
{

/**
 * HTTP Version wrapper
 */
struct Version
{
  static const std::size_t STRING_LEN = 8; /* strlen("HTTP/X.X") */
  static const std::string HEADER;

  /**
   * Parse an HTTP version string.
   * @param str The version string in the format HTTP/X.X
   */
  static std::pair<Version, bool> from_string(const std::string &str);

  operator std::string() const;

  std::uint8_t major;
  std::uint8_t minor;
};

}  // namespace microhttp::http
