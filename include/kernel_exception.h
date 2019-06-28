// Copyright 2019 Victor Barbu.

#pragma once

#include <stdexcept>

#include <errno.h>
#include <cstring>

namespace microloop {

// Wrapper around error codes returned by system calls.
class KernelException : public std::runtime_error {
 public:
  explicit KernelException(int err) : std::runtime_error{strerror(err)} {}
};

}  // namespace microloop
