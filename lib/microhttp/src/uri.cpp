//
// Copyright (c) 2020 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/uri.h"

#include <cctype>
#include <charconv>
#include <cstring>
#include <iterator>
#include <string>

namespace microhttp::http
{

static Uri decode_uri(std::string_view uri)
{
  return Uri{};
}

std::optional<Uri> UriParser::parse(std::string_view sv)
{
  sv_ = sv;

  if (!parse_scheme())
  {
    return std::nullopt;
  }

  if (!parse_hier_part())
  {
    return std::nullopt;
  }

  return uri_;
}

bool UriParser::parse_scheme()
{
  static constexpr auto is_valid_char
      = [](unsigned char c) { return c == '+' || c == '-' || c == '.'; };
  static constexpr auto is_colon = [](unsigned char c) { return c == ':'; };

  std::optional<char> c = consume_if(isalpha);
  if (c == std::nullopt)
  {
    return false;
  }

  uri_.scheme.push_back(*c);

  while (auto d = consume_if(isalpha, isdigit, is_valid_char))
  {
    uri_.scheme.push_back(*d);
  }

  if (!consume_if(is_colon))
  {
    error_ = Error::INVALID_SCHEME;
    return false;
  }

  return true;
}

bool UriParser::parse_hier_part()
{
  if (sv_.substr(0, 2) == "//")
  {
    ignore(2);
    parse_authority();
  }
}

bool UriParser::parse_authority()
{
  if (sv_.find('@') != std::string_view::npos && !parse_userinfo())
  {
    error_ = Error::INVALID_USERINFO;
    return false;
  }

  if (!parse_host())
  {
    return false;
  }

  if (curr() == ':' && !parse_port())
  {
    error_ = Error::INVALID_PORT;
    return false;
  }

  return true;
}

bool UriParser::parse_userinfo()
{
  while (true)
  {
    if (auto pct_encoded = parse_pct_encoded(); pct_encoded)
    {
      uri_.userinfo.push_back(*pct_encoded);
    }
    else if (is_sub_delim(curr()) || curr() == ':' || is_unreserved(curr()))
    {
      uri_.userinfo.push_back(consume());
    }
    else
    {
      break;
    }
  }

  if (curr() != '@')
  {
    error_ = Error::INVALID_USERINFO;
    return false;
  }

  ignore(1);

  return true;
}

bool UriParser::parse_host()
{
  return parse_ip_literal() || parse_ipv4_address() || parse_reg_name();
}

bool UriParser::parse_ip_literal()
{
  if (curr() != '[')
  {
    return false;
  }

  ignore(1);

  if ((curr() == 'v' || curr() == 'V'))
  {
    if (!parse_ipvfuture())
    {
      error_ = Error::INVALID_IPVFUTURE_ADDR;
      return false;
    }
  }
  else if (!parse_ipv6_address())
  {
    error_ = Error::INVALID_IPV6_ADDR;
    return false;
  }

  if (curr() != ']')
  {
    error_ = Error::INVALID_IP_LITERAL;
    return false;
  }

  ignore(1);  // consume the "]"
  return true;
}

bool UriParser::parse_reg_name()
{
  while (true)
  {
    if (auto hex_encoded = parse_pct_encoded(); hex_encoded)
    {
      uri_.host.push_back(*hex_encoded);
    }
    else if (is_unreserved(curr()) || is_sub_delim(curr()))
    {
      uri_.host.push_back(consume());
    }
    else
    {
      return true;
    }
  }
}

bool UriParser::parse_ipv4_address()
{
  std::string ipv4_address;
  std::string dec_octet;

  for (auto i = 0; i < 4; i++)
  {
    while (auto c = consume_if(isdigit))
    {
      dec_octet.push_back(*c);
    }

    std::uint32_t octet;
    std::from_chars(dec_octet.c_str(), dec_octet.c_str() + dec_octet.size(), octet);

    if (octet > 255)
    {
      error_ = Error::INVALID_IPV4_ADDR;
      return false;
    }

    ipv4_address.append(dec_octet);
    dec_octet.clear();

    if (i < 3 && curr() != '.')
    {
      error_ = Error::INVALID_IPV4_ADDR;
      return false;
    }
    else
    {
      ipv4_address.push_back(consume());
    }
  }

  uri_.host = ipv4_address;

  return true;
}

bool UriParser::parse_ipv6_address()
{
  return false;
}

bool UriParser::parse_ipvfuture()
{
  if ((curr() != 'v' && curr() != 'V'))
  {
    return false;
  }

  std::string ip = "[";
  ip.push_back(consume());

  if (auto v = consume_if(is_hexdigit); v)
  {
    ip.push_back(*v);
  }
  else
  {
    error_ = Error::INVALID_IP_VERSION;
    return false;
  }

  if (curr() != '.')
  {
    error_ = Error::INVALID_IPVFUTURE_ADDR;
    return false;
  }

  ip.push_back(consume());

  static constexpr auto is_colon = [](char c) { return c == ':'; };

  while (auto c = consume_if(is_unreserved, is_sub_delim, is_colon))
  {
    ip.push_back(*c);
  }

  ip.push_back(']');

  uri_.host = ip;
  return true;
}

bool UriParser::parse_port()
{
  if (curr() != ':')
  {
    return false;
  }

  ignore(1);

  std::string port_str;
  while (auto c = consume_if(isdigit))
  {
    port_str.push_back(*c);
  }

  std::uint32_t port;
  std::from_chars(port_str.c_str(), port_str.c_str() + port_str.size(), port);

  if (port > std::numeric_limits<std::uint16_t>::max())
  {
    error_ = Error::INVALID_PORT;
    return false;
  }

  uri_.port = port;
  return true;
}

std::optional<char> UriParser::parse_pct_encoded()
{
  if (curr() != '%')
  {
    return std::nullopt;
  }

  ignore(1);

  char hex_encoded[2]{};

  for (auto i = 0; i < 2; i++)
  {
    if (is_hexdigit(curr()))
    {
      hex_encoded[i] = consume();
    }
  }

  std::uint32_t decoded;
  std::from_chars(std::begin(hex_encoded), std::end(hex_encoded), decoded, 16);

  if (decoded > 255)
  {
    error_ = Error::INVALID_PCT_ENCODING;
    return std::nullopt;
  }

  return static_cast<char>(decoded);
}

char UriParser::peek() const
{
  return sv_[1];
}

char UriParser::curr() const
{
  return sv_.front();
}

char UriParser::consume()
{
  auto c = curr();
  sv_.remove_prefix(1);

  return c;
}

std::string UriParser::consume(std::size_t count)
{
  std::string prefix{sv_.substr(0, count)};
  sv_.remove_prefix(count);

  return prefix;
}

void UriParser::ignore(std::size_t count)
{
  sv_.remove_prefix(count);
}

bool UriParser::eof() const
{
  return sv_.empty();
}

}  // namespace microhttp::http
