//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "utils/error.h"

#include <cstring>

namespace microloop::utils::error
{

std::string strerror(std::uint32_t err)
{
  /*
   * See http://man7.org/linux/man-pages/man3/strerror.3.html#NOTES for explanation
   */
  static const std::size_t safe_err_len = 1024;
  char buf[safe_err_len] = {};
  ::strerror_r(errno, buf, safe_err_len);

  return std::string{buf};
}

}  // namespace microloop::utils::error
