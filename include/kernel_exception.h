// Copyright 2019 Victor Barbu.

#pragma once

#include <cstring>
#include <errno.h>
#include <stdexcept>

namespace microloop {

// Wrapper around error codes returned by system calls.
class KernelException : public std::runtime_error {
public:
  explicit KernelException(int err) : std::runtime_error { strerror(err) }
  {}
};

}  // namespace microloop
