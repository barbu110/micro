//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <map>
#include <string_view>

namespace microhttp::http
{

/**
 * \brief The HTTP status codes defined in RFC7231 and RFC7233.
 */
enum StatusCode : std::uint16_t
{
  CONTINUE = 100,
  SWITCHING_PROTOCOLS = 101,
  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  NON_AUTHORITATIVE_INFO = 203,
  NO_CONTENT = 204,
  RESET_CONTENT = 205,
  PARTIAL_CONTENT = 206,
  MULTIPLE_CHOICES = 300,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  NOT_MODIFIED = 304,
  USE_PROXY = 305,
  TEMPORARY_REDIRECT = 307,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  PAYMENT_REQUIRED = 402, /* not used */
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  NOT_ACCEPTABLE = 406,
  PROXY_AUTHENTICATION = 407,
  REQUEST_TIMEOUT = 408,
  CONFLICT = 409,
  GONE = 410,
  LENGTH_REQUIRED = 411,
  PRECONDITION_FAILED = 412,
  PAYLOAD_TOO_LARGE = 413,
  URI_TOO_LONG = 414,
  UNSUPPORTED_MEDIA_TYPE = 415,
  RANGE_NOT_SATISFIABBLE = 416,
  EXPECTATION_FAILED = 417,
  UPGRADE_REQUIRED = 426,
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNAVAILABLE = 503,
  GATEWAY_TIMEOUT = 504,
  HTTP_VERSION_NOT_SUPPORTED = 505,
};

/**
 * \brief The mapping between HTTP status codes and reason phrases.
 */
static const std::map<std::uint16_t, std::string_view> http_reason_phrases{
  std::make_pair(StatusCode::CONTINUE, "Continue"),
  std::make_pair(StatusCode::SWITCHING_PROTOCOLS, "Switching Protocols"),
  std::make_pair(StatusCode::OK, "OK"),
  std::make_pair(StatusCode::CREATED, "Created"),
  std::make_pair(StatusCode::ACCEPTED, "Accepted"),
  std::make_pair(StatusCode::NON_AUTHORITATIVE_INFO, "Non-Authoritative Information"),
  std::make_pair(StatusCode::NO_CONTENT, "No Content"),
  std::make_pair(StatusCode::RESET_CONTENT, "Reset Content"),
  std::make_pair(StatusCode::PARTIAL_CONTENT, "Partial Content"),
  std::make_pair(StatusCode::MULTIPLE_CHOICES, "Multiple Choices"),
  std::make_pair(StatusCode::MOVED_PERMANENTLY, "Moved Permanently"),
  std::make_pair(StatusCode::FOUND, "Found"),
  std::make_pair(StatusCode::SEE_OTHER, "See Other"),
  std::make_pair(StatusCode::NOT_MODIFIED, "Not Modified"),
  std::make_pair(StatusCode::USE_PROXY, "Use Proxy"),
  std::make_pair(StatusCode::TEMPORARY_REDIRECT, "Temporary Redirect"),
  std::make_pair(StatusCode::BAD_REQUEST, "Bad Request"),
  std::make_pair(StatusCode::UNAUTHORIZED, "Unauthorized"),
  std::make_pair(StatusCode::PAYMENT_REQUIRED, "Payment Required"),
  std::make_pair(StatusCode::FORBIDDEN, "Forbidden"),
  std::make_pair(StatusCode::NOT_FOUND, "Not Found"),
  std::make_pair(StatusCode::METHOD_NOT_ALLOWED, "Method Not Allowed"),
  std::make_pair(StatusCode::NOT_ACCEPTABLE, "Not Acceptable"),
  std::make_pair(StatusCode::PROXY_AUTHENTICATION, "Proxy Authentication Required"),
  std::make_pair(StatusCode::REQUEST_TIMEOUT, "Request Time-out"),
  std::make_pair(StatusCode::CONFLICT, "Conflict"),
  std::make_pair(StatusCode::GONE, "Gone"),
  std::make_pair(StatusCode::LENGTH_REQUIRED, "Length Required"),
  std::make_pair(StatusCode::PRECONDITION_FAILED, "Precondition Failed"),
  std::make_pair(StatusCode::PAYLOAD_TOO_LARGE, "Request Entity Too Large"),
  std::make_pair(StatusCode::URI_TOO_LONG, "Request-URI Too Large"),
  std::make_pair(StatusCode::UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"),
  std::make_pair(StatusCode::RANGE_NOT_SATISFIABBLE, "Requested range not satisfiable"),
  std::make_pair(StatusCode::EXPECTATION_FAILED, "Expectation Failed"),
  std::make_pair(StatusCode::INTERNAL_SERVER_ERROR, "Internal Server Error"),
  std::make_pair(StatusCode::NOT_IMPLEMENTED, "Not Implemented"),
  std::make_pair(StatusCode::BAD_GATEWAY, "Bad Gateway"),
  std::make_pair(StatusCode::SERVICE_UNAVAILABLE, "Service Unavailable"),
  std::make_pair(StatusCode::GATEWAY_TIMEOUT, "Gateway Time-out"),
  std::make_pair(StatusCode::HTTP_VERSION_NOT_SUPPORTED, "HTTP Version not supported"),
};

}  // namespace microhttp::http
