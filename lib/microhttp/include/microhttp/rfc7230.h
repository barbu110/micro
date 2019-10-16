//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microhttp/version.h"

#include <optional>
#include <string_view>

namespace microhttp::http
{

struct StartLineData;
struct HeaderLineData;

class RFC7230
{
protected:
  /**
   * \brief Parse the start line of an HTTP request according to RFC 7230 standard. Note that this
   * string must include the CRLF token, otherwise parsing will fail.
   * \param str The view into the HTTP request string representing the entire start line.
   * \returns Information extracted from the start line, if it was valid, or an empty value
   * otherwise.
   */
  std::optional<StartLineData> parse_start_line(std::string_view str) noexcept;

  /**
   * \brief Parse a header line of an HTTP request according to RFC 7230 standard. Note that this
   * string must include the CRLF token at the end of the line, otherwise parsing will fail.
   *
   * This version of the standard parser does not support header values extending on more than one
   * line as specified by Section 3.2 of the standard.
   *
   * \param str The view into the HTTP request string representing an entire header line.
   * \returns Information extracted from the given header line, or an empty value if the line was
   * not valid according to the standard and the limitations imposed by this implementation.
   */
  std::optional<HeaderLineData> parse_header_line(std::string_view str);
};

/**
 * \brief Information extracted from the start line of an HTTP request.
 */
struct StartLineData
{
  /// The HTTP method
  std::string_view method;

  /// The request target
  std::string_view request_target;

  /// The HTTP version used by the client
  Version version;
};

/**
 * \brief Information extracted from a header line of an HTTP request.
 */
struct HeaderLineData
{
  /// The name of the header
  std::string_view name;

  /// The value of the header
  std::string_view value;
};

}  // namespace microhttp::http
