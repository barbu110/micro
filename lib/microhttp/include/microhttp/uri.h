//
// Copyright (c) 2020 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace microhttp::http
{

/// Structured URI information.
struct Uri
{
  /// The scheme of the URI as described by RFC 3986, Section 3.1.
  std::string scheme;

  /// The user information segment of the authority as described by RFC 3986, Section 3.2.1.
  std::string userinfo;

  /// The host segment of the authority as described by RFC 3986, Section 3.2.2.
  std::string host;

  std::int16_t port;

  /// The path part of the URI as described by RFC 3986, Section 3.3.
  std::string path;

  /// The query part of the URI as described by RFC 3986, Section 3.4.
  std::string query;

  /// The fragment part of the URI as described by RFC 3986, Section 3.5.
  std::string fragment;
};

/**
 * \brief Parse a URI according to the syntax described in RFC 3986.
 * \param uri The string URI. Can be a relative or an absolute URI.
 * \return A decoded URI object.
 */
static Uri decode_uri(std::string_view uri);

/**
 * \brief Encode a URI object to string format as described in RFC 3986.
 * \param uri The structured URI object.
 * \return The encoded URI string.
 */
static std::string encode_uri(const Uri &uri);

class UriParser
{
public:
  enum Error
  {
    NONE = 0,
    INVALID_SCHEME,
    INVALID_USERINFO,
    INVALID_IP_LITERAL,
    INVALID_IPV4_ADDR,
    INVALID_IPV6_ADDR,
    INVALID_IP_VERSION,
    INVALID_IPVFUTURE_ADDR,
    INVALID_PORT,
    INVALID_PCT_ENCODING,
  };

  std::optional<Uri> parse(std::string_view sv);
  Error error() const
  {
    return error_;
  }

private:
  bool parse_scheme();
  bool parse_hier_part();
  bool parse_authority();
  bool parse_userinfo();
  bool parse_host();
  bool parse_ip_literal();
  bool parse_ipv4_address();
  bool parse_ipv6_address();
  bool parse_ipvfuture();
  bool parse_reg_name();
  bool parse_port();
  bool parse_query();
  bool parse_fragment();
  std::optional<char> parse_pct_encoded();

  char peek() const;
  char curr() const;
  char consume();
  std::string consume(std::size_t count);
  void ignore(std::size_t count = 1);
  bool eof() const;

  std::optional<char> consume_if()
  {
    return std::nullopt;
  }

  template <typename UnaryPredicate, typename... Others>
  std::optional<char> consume_if(UnaryPredicate p, Others... others)
  {
    if (eof())
    {
      return std::nullopt;
    }

    return p(curr()) == 0 ? consume_if(others...) : consume();
  }

  static bool is_reserved(char c) noexcept
  {
    return is_sub_delim(c) || is_gen_delim(c);
  }

  static bool is_sub_delim(char c) noexcept
  {
    /*
     * Sub delimiters defined in Section 2.2. Reserved Characters of RFC 3986.
     */
    static const std::string sub_delims = "!$&'()*+,;=";

    return sub_delims.find(c) != std::string::npos;
  }

  static bool is_gen_delim(char c) noexcept
  {
    /*
     * General delimiters as defined in Section 2.2. Reserved Characters of RFC 3986.
     */
    static const std::string gen_delims = ":/?#[]@";

    return gen_delims.find(c) != std::string::npos;
  }

  static bool is_unreserved(unsigned char c) noexcept
  {
    /*
     * Unreserved characters as defined in Section 2.3. Unreserved Characters of RFC 3986.
     */
    return std::isalpha(c) || std::isdigit(c) || c == '-' || c == '.' || c == '_' || c == '~';
  }

  static bool is_hexdigit(unsigned char c) noexcept
  {
    return std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  }

  std::string_view sv_;
  Error error_ = NONE;
  Uri uri_;
};

}  // namespace microhttp::http
