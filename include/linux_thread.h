// Copyright 2019 Victor Barbu.

#pragma once

#include "./kernel_exception.h"

#include <errno.h>
#include <functional>
#include <pthread.h>
#include <signal.h>

namespace microloop {

class LinuxThread {
  public:
  using worker_fn = std::function<void*(void*)>;

  LinuxThread(worker_fn worker)
      : worker_ { worker }
  {
  }

  void run(void* args)
  {
    pthread_t thread;
    pthread_attr_t thread_attr;
    int error_code;

    error_code = pthread_attr_init(&thread_attr);
    if (error_code != 0) {
      throw KernelException(error_code);
    }

    error_code = pthread_attr_setdetachstate(&thread_attr,
        PTHREAD_CREATE_DETACHED);
    if (error_code != 0) {
      throw KernelException(error_code);
    }

    error_code = pthread_create(&thread, &thread_Attr, worker_.target, args);
    if (error_code != 0) {
      throw KernelException(error_code);
    }
  }

  private:
  worker_fn worker_;
};

}  // namespace microloop
