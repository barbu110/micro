//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <string>

namespace microloop::utils::error
{

/**
 * Get an std::string representation of the given error number. This method is to be used for
 * wrapping kernel error codes in a safe string.
 * @param  err The error number as retrieved from syscalls or the errno macro.
 * @return A safe std::string representation of the given error number.
 */
std::string strerror(std::uint32_t err);

}
