// Copyright 2019 Stefan Silviu-Alexandru

#include <memory>
#include <functional>
#include <iostream>

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

PThread::PThread(ThreadRoutine targetRoutine, void* routineData, const pthread_attr_t* attrs) {
  int err = pthread_create(&handle, attrs, targetRoutine, routineData);
  if (err) throw PThreadException("pthread_create", err);
}

PThread::PThread(ThreadRoutine routine) : PThread(routine, nullptr, nullptr) {}

PThread::PThread(ThreadRoutine routine, const PThreadAttributes& attrs) :
  PThread(routine, nullptr, attrs.getUnderlyingData()) {}

PThread::PThread(std::function<void()> routine) :
    PThread(routineWrapperNoRet, &routine, nullptr) {}

PThread::PThread(std::function<void()> routine, const PThreadAttributes& attrs) :
    PThread(routineWrapperNoRet, &routine, attrs.getUnderlyingData()) {}

PThreadAttributes::PThreadAttributes() {
  int err = pthread_attr_init(&attr);
  if (err) throw PThreadException("pthread_attr_init", err);
}

PThreadAttributes::~PThreadAttributes() {
  int err = pthread_attr_destroy(&attr);
  if (err) std::cerr << "(swallowed error in destructor) pthread_attr_destroy: " << err;
}

#pragma clang diagnostic pop

PThread PThread::makeWithRet(std::function<void*()> routine) {
  return PThread(routineWrapperWithRet, &routine, nullptr);
}

PThread PThread::makeWithRet(std::function<void*()> routine, const PThreadAttributes& attrs) {
  return PThread(routineWrapperWithRet, &routine, attrs.getUnderlyingData());
}

void PThread::detach() {
  // Avoid unspecified behaviour
  if (detached) throw std::logic_error("Attempt to detach already detached thread");
  int err = pthread_detach(handle);
  if (err) throw PThreadException("pthread_detach", err);
  detached = true;
}

void PThread::exit(void* retVal) noexcept {
  pthread_exit(retVal);
}

bool PThread::operator==(const PThread& other) const noexcept {
  return pthread_equal(handle, other.handle);
}

bool PThread::operator!=(const PThread& other) const noexcept {
  return !operator==(other);
}

PThread::PThread(pthread_t otherHandle) noexcept : handle(otherHandle) {}

PThread PThread::self() noexcept {
  return PThread(pthread_self());
}

const pthread_attr_t* PThreadAttributes::getUnderlyingData() const noexcept {
  return &attr;
}

#define ATTR_SETTER(EnumClass, pthread_function) do { \
  int err = pthread_function(&attr, state); \
  if (err) throw PThreadException(#pthread_function, err); \
  return *this; \
} while(0)

#define ATTR_GETTER(EnumClass, ParamType, pthread_function) do { \
  ParamType state; \
  int err = pthread_function(&attr, &state); \
  if (err) throw PThreadException(#pthread_function, err); \
  return EnumClass(state); \
} while(0)

PThreadAttributes& PThreadAttributes::setDetachState(AttrDetachState state) {
  ATTR_SETTER(AttrDetachState, pthread_attr_setdetachstate);
}

AttrDetachState PThreadAttributes::getDetachState() const {
  ATTR_GETTER(AttrDetachState, int, pthread_attr_getdetachstate);
}

PThreadAttributes& PThreadAttributes::setScope(AttrThreadScope state) {
  ATTR_SETTER(AttrThreadScope, pthread_attr_setscope);
}

AttrThreadScope PThreadAttributes::getScope() const {
  ATTR_GETTER(AttrThreadScope, int, pthread_attr_getscope);
}

PThreadAttributes& PThreadAttributes::setInheritScheduler(AttrInheritScheduler state) {
  ATTR_SETTER(AttrInheritScheduler, pthread_attr_setinheritsched);
}

AttrInheritScheduler PThreadAttributes::getInheritScheduler() const {
  ATTR_GETTER(AttrInheritScheduler, int, pthread_attr_getinheritsched);
}

PThreadAttributes& PThreadAttributes::setSchedulingPolicy(AttrSchedulingPolicy state) {
  ATTR_SETTER(AttrSchedulingPolicy, pthread_attr_setschedpolicy);
}

AttrSchedulingPolicy PThreadAttributes::getSchedulingPolicy() const {
  ATTR_GETTER(AttrSchedulingPolicy, int, pthread_attr_getschedpolicy);
}

PThreadAttributes& PThreadAttributes::setSchedulingPriority(int prio) {
  sched_param schedParam { prio };
  int err = pthread_attr_setschedparam(&attr, &schedParam);
  if (err) throw PThreadException("pthread_attr_setschedparam", err);
  return *this;
}

int PThreadAttributes::getSchedulingPriority() const {
  sched_param state { 0 };
  int err = pthread_attr_getschedparam(&attr, &state);
  if (err) throw PThreadException("pthread_attr_getschedparam", err);
  return state.sched_priority;
}

PThreadAttributes& PThreadAttributes::setGuardSize(std::size_t state) {
  ATTR_SETTER(std::size_t, pthread_attr_setguardsize);
}

std::size_t PThreadAttributes::getGuardSize() const {
  ATTR_GETTER(std::size_t, std::size_t, pthread_attr_getguardsize);
}

PThreadAttributes& PThreadAttributes::setStackSize(std::size_t state) {
  ATTR_SETTER(std::size_t, pthread_attr_setstacksize);
}

std::size_t PThreadAttributes::getStackSize() const {
  ATTR_GETTER(std::size_t, std::size_t, pthread_attr_getstacksize);
}

PThreadAttributes& PThreadAttributes::setStackInfo(void* addr, std::size_t size) {
  int err = pthread_attr_setstack(&attr, addr, size);
  if (err) throw PThreadException("pthread_attr_setstack", err);
  return *this;
}

StackInfo PThreadAttributes::getStackInfo() const {
  StackInfo info { nullptr, 0 };
  int err = pthread_attr_getstack(&attr, &info.addr, &info.size);
  if (err) throw PThreadException("pthread_attr_getstack", err);
  return info;
}

#undef ATTR_SETTER
#undef ATTR_GETTER

}
