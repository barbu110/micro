// Copyright 2019 Stefan Silviu-Alexandru

#ifndef MICROLOOP_PTHREAD_WRAPPER_H
#define MICROLOOP_PTHREAD_WRAPPER_H

#include <pthread.h>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <cstring>

namespace microloop {
class PThreadException : public std::runtime_error {
  int error;
public:
  explicit PThreadException(const std::string& source, int error) :
      std::runtime_error(source + " error: " + strerror(error)), error(error) {}
};

using ThreadRoutine = void*(void*);

class PThread {
  pthread_t handle;

  PThread(ThreadRoutine targetRoutine, void* routineData);

public:
  explicit PThread(ThreadRoutine routine);

  explicit PThread(std::function<void()> routine);

  static PThread makeWithRet(std::function<void*()> routine);

  void join();
};

}

#endif // MICROLOOP_PTHREAD_WRAPPER_H
