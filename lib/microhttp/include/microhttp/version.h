//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <string_view>

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
   * \brief Parse an HTTP version string view.
   * \param sv The string view containing the textual HTTP version in the format HTTP/X.X.
   * \returns A pair with two elements; first is an HTTP version object, and the second is a flag
   * indicating whether the parsing failed or not.
   */
  static std::pair<Version, bool> from_string(std::string_view sv);

  operator std::string() const;

  std::uint8_t major;
  std::uint8_t minor;
};

bool operator==(const Version &, const Version &);

}  // namespace microhttp::http
