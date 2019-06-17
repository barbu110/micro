// Copyright 2019 Stefan Silviu-Alexandru

#ifndef MICROLOOP_PTHREAD_WRAPPER_H
#define MICROLOOP_PTHREAD_WRAPPER_H

#include <pthread.h>
#include <stdexcept>
#include <functional>
#include <memory>
#include <cstring>

#define noreturn __attribute((noreturn))

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
  bool detached = false;

  PThread(ThreadRoutine targetRoutine, void* routineData);

public:
  explicit PThread(ThreadRoutine routine);

  explicit PThread(std::function<void()> routine);

  static PThread makeWithRet(std::function<void*()> routine);

  static noreturn void exit(void* retVal = nullptr) noexcept;

  template<typename T = void>
  T* join() {
    void* retValPtr;
    int err = pthread_join(handle, &retValPtr);
    if (err) throw PThreadException("pthread_join", err);
    return static_cast<T*>(retValPtr);
  }

  void detach();
};

}

#endif // MICROLOOP_PTHREAD_WRAPPER_H
