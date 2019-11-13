//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microhttp/constants.h"
#include "microhttp/status_codes.h"
#include "microhttp/version.h"
#include "microloop/buffer.h"

#include <map>
#include <optional>
#include <sstream>
#include <string>

namespace microhttp::http
{

class HttpResponse
{
public:
  HttpResponse(const microloop::Buffer &content = microloop::Buffer{},
      microhttp::http::StatusCode status_code = StatusCode::OK) :
      status_code_{status_code}, content_{content}
  {
    set_header("Content-Length", content.size());
  }

  /**
   * \brief Set the HTTP version embedded within this response.
   * \param version The version to be set.
   */
  void set_http_version(const microhttp::http::Version &version)
  {
    http_version_ = version;
  }

  /**
   * \brief Get the already set HTTP version on this HTTP response. If no version has been
   * explicitly set, the default will be returned.
   */
  microhttp::http::Version http_version() const
  {
    return http_version_;
  }

  /**
   * \brief Set the status code on this response.
   */
  void set_status_code(microhttp::http::StatusCode status_code)
  {
    status_code_ = status_code;
  }

  /**
   * \brief Get the status code set on this response.
   */
  microhttp::http::StatusCode status_code() const
  {
    return status_code_;
  }

  /**
   * \brief Get the HTTP reason phrase associated with the already set status code.
   */
  auto reason_phrase() const
  {
    auto entry = http_reason_phrases.find(status_code_);
    return entry->second;
  }

  /**
   * \brief Set a header on this HTTP response with a numeric value.
   * \param name The name of the header (case-insensitive).
   * \param value The numeric value of the header.
   */
  template <typename T>
  std::enable_if_t<std::is_arithmetic_v<T>> set_header(const std::string &name, T value)
  {
    set_header(name, std::to_string(value));
  }

  /**
   * \brief Set a header on this HTTP response with a string or string-convertible value.
   * \param name The name of the header (case-insensitive).
   * \param value The value of the header.
   */
  template <typename T>
  std::enable_if_t<not std::is_arithmetic_v<T>> set_header(const std::string &name, T value)
  {
    headers_[name] = static_cast<std::string>(value);
  }

  /**
   * \brief Get the value of a header set on this HTTP response.
   * \param name The name of the header (case-insensitive).
   * \return Optional value of the header. nullopt if no such header is set on this response.
   */
  std::optional<std::string> header(const std::string &name) const
  {
    if (headers_.find(name) == headers_.end())
    {
      return std::nullopt;
    }

    return headers_.at(name);
  }

  /**
   * \brief Get the content of this response.
   */
  microloop::Buffer &content()
  {
    return content_;
  }

  /**
   * \brief Get the read-only content of this response.
   */
  const microloop::Buffer &content() const
  {
    return content_;
  }

  /**
   * \brief Get the HTTP response in the specified format.
   *
   * This function is to be used for retrieving the response in a serialized version. At least two
   * specializations exist:
   *  1) format<std::string>()
   *  2) format<microloop::Buffer>()
   *
   * \return The serialized http response.
   */
  template <typename Format>
  Format format() const;

private:
  microhttp::http::Version http_version_{1, 1};
  microhttp::http::StatusCode status_code_;
  std::map<std::string, std::string> headers_;
  microloop::Buffer content_;
};

template <>
std::string HttpResponse::format<>() const
{
  std::ostringstream ss;
  ss << static_cast<std::string>(http_version()) << " " << status_code() << " " << reason_phrase()
     << constants::crlf;

  for (const auto &[name, value] : headers_)
  {
    ss << name << ": " << value << constants::crlf;
  }

  ss << constants::crlf << content().str_view();

  return ss.str();
}

template <>
microloop::Buffer HttpResponse::format<>() const
{
  return microloop::Buffer{format<std::string>().c_str()};
}

}  // namespace microhttp::http
