// Copyright 2019 Stefan Silviu-Alexandru

#include <memory>
#include <functional>

#include "pthread_wrapper.h"

void* routineWrapperWithRet(void* target) {
  return (*static_cast<std::function<void*()>*>(target))();
}

void* routineWrapperNoRet(void* target) {
  (*static_cast<std::function<void()>*>(target))();
  return nullptr;
}

namespace microloop {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

PThread::PThread(ThreadRoutine targetRoutine, void* routineData) {
  int err = pthread_create(&handle, nullptr, targetRoutine, routineData);
  if (err) throw PThreadException("pthread_create", err);
}

PThread::PThread(ThreadRoutine routine) : PThread(routine, nullptr) {}

PThread::PThread(std::function<void()> routine) :
    PThread(routineWrapperNoRet, reinterpret_cast<void*>(&routine)) {}

#pragma clang diagnostic pop

PThread PThread::makeWithRet(std::function<void*()> routine) {
  void* stdFunPtr = reinterpret_cast<void*>(&routine);
  return PThread(routineWrapperWithRet, stdFunPtr);
}

void PThread::detach() {
  if (detached) throw std::logic_error("Attempt to detach already detached thread");
  int err = pthread_detach(handle);
  if (err) throw PThreadException("pthread_detach", err);
  detached = true;
}

void PThread::exit(void* retVal) noexcept {
  pthread_exit(retVal);
}

}
