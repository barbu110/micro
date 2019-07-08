//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <kernel_exception.h>
#include <string>

namespace microloop::errors {

class IOException : public microloop::KernelException {
public:
  IOException(int fd, int err) :
    KernelException{err}, fd{fd}
  {}

  int get_fd() const
  {
    return fd;
  }

private:
  int fd;
};

}
