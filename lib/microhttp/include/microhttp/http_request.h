//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microhttp/version.h"
#include "microloop/buffer.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <optional>
#include <string>

namespace microhttp::http
{

class HttpRequest
{
public:
  HttpRequest();

  HttpRequest(std::string method, std::string uri);

  Version get_http_version() const
  {
    return http_version;
  }

  void set_http_version(Version v)
  {
    http_version = v;
  }

  bool set_http_version(const std::string &version_string)
  {
    auto [version, valid] = Version::from_string(version_string);
    if (!valid)
    {
      return false;
    }

    http_version = version;
    return true;
  }

  std::string get_http_method() const
  {
    return http_method;
  }

  void set_http_method(const std::string &method)
  {
    http_method = method;

    std::transform(http_method.begin(), http_method.end(), http_method.begin(),
        [](auto c) { return std::tolower(c); });
  }

  std::string get_uri() const
  {
    return uri;
  }

  void set_uri(const std::string &uri)
  {
    /*
     * TODO Apply URI normalization procedures described in RFC7230.
     */

    this->uri = uri;
  }

  std::pair<std::string, bool> get_header(std::string header_name) const noexcept
  {
    std::transform(header_name.begin(), header_name.end(), header_name.begin(),
        [](auto c) { return std::tolower(c); });

    auto header = headers.find(header_name);
    if (header == headers.cend())
    {
      return std::make_pair("", false);
    }

    return std::make_pair(header->second, true);
  }

  void remove_header(const std::string &header_name) noexcept
  {
    headers.erase(header_name);
  }

  void set_header(const std::string &name, const std::string &value) noexcept
  {
    std::string normalized_name = name;
    std::transform(normalized_name.begin(), normalized_name.end(), normalized_name.begin(),
        [](auto c) { return std::tolower(c); });

    headers[normalized_name] = value;
  }

  const std::map<std::string, std::string> &get_headers() const
  {
    return headers;
  }

  const microloop::Buffer &get_body() const noexcept
  {
    return body;
  }

  microloop::Buffer &get_body() noexcept
  {
    return body;
  }

  std::string get_body_string() const noexcept
  {
    return body.str();
  }

  std::optional<std::size_t> get_content_length() const noexcept
  {
    auto [content_length, found] = get_header("Content-Length");

    if (!found)
    {
      return std::nullopt;
    }

    return std::atoll(content_length.c_str());
  }

private:
  Version http_version;
  std::string http_method;
  std::string uri;
  std::map<std::string, std::string> headers;
  microloop::Buffer body;
};

}  // namespace microhttp::http
