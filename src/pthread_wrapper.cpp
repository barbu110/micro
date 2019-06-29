// Copyright 2019 Stefan Silviu-Alexandru

#include "pthread_wrapper.h"

#include <functional>
#include <iostream>
#include <memory>

void* routine_wrapper_with_ret(void* target)
{
  return (*static_cast<std::function<void*()>*>(target))();
}

void* routine_wrapper_null_ret(void* target)
{
  (*static_cast<std::function<void()>*>(target))();
  return nullptr;
}

void routine_wrapper_no_ret(void* target)
{
  (*static_cast<std::function<void()>*>(target))();
}

namespace microloop {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Thread::Thread(ThreadRoutine target_routine, void* routine_data, const pthread_attr_t* attrs)
{
  int err = pthread_create(&handle, attrs, target_routine, routine_data);
  if (err)
    throw ThreadException("pthread_create", err);
}

Thread::Thread(ThreadRoutine routine) : Thread(routine, nullptr, nullptr)
{}

Thread::Thread(ThreadRoutine routine, const ThreadAttributes& attrs) :
    Thread(routine, nullptr, attrs.underlying_data())
{}

Thread::Thread(std::function<void()> routine) : Thread(routine_wrapper_null_ret, &routine, nullptr)
{}

Thread::Thread(std::function<void()> routine, const ThreadAttributes& attrs) :
    Thread(routine_wrapper_null_ret, &routine, attrs.underlying_data())
{}

ThreadAttributes::ThreadAttributes()
{
  int err = pthread_attr_init(&attr);
  if (err)
    throw ThreadException("pthread_attr_init", err);
}

ThreadAttributes::~ThreadAttributes()
{
  int err = pthread_attr_destroy(&attr);
  if (err)
    std::cerr << "(swallowed error in destructor) pthread_attr_destroy: " << err;
}

SpinLock::SpinLock(LockShareState shareState)
{
  int err = pthread_spin_init(&lock, shareState);
  if (err)
    throw ThreadException("pthread_spin_init", err);
}

#pragma clang diagnostic pop

Thread Thread::make_with_ret(std::function<void*()> routine)
{
  return Thread(routine_wrapper_with_ret, &routine, nullptr);
}

Thread Thread::make_with_ret(std::function<void*()> routine, const ThreadAttributes& attrs)
{
  return Thread(routine_wrapper_with_ret, &routine, attrs.underlying_data());
}

void Thread::detach()
{
  // Avoid unspecified behaviour
  if (detached)
    throw std::logic_error("Attempt to detach already detached thread");
  int err = pthread_detach(handle);
  if (err)
    throw ThreadException("pthread_detach", err);
  detached = true;
}

void Thread::exit(void* ret_val) noexcept
{
  pthread_exit(ret_val);
}

bool Thread::operator==(const Thread& other) const noexcept
{
  return pthread_equal(handle, other.handle);
}

bool Thread::operator!=(const Thread& other) const noexcept
{
  return !operator==(other);
}

Thread::Thread(pthread_t other_handle) noexcept : handle(other_handle)
{}

Thread Thread::self() noexcept
{
  return Thread(pthread_self());
}

ThreadCancelState Thread::set_cancel_state(ThreadCancelState new_state)
{
  int old_state;
  int err = pthread_setcancelstate(new_state, &old_state);
  if (err)
    throw ThreadException("pthread_setcancelstate", err);
  return ThreadCancelState(old_state);
}

ThreadCancelType Thread::set_cancel_type(ThreadCancelType new_type)
{
  int old_type;
  int err = pthread_setcanceltype(new_type, &old_type);
  if (err)
    throw ThreadException("pthread_setcanceltype", err);
  return ThreadCancelType(old_type);
}

void Thread::cancel() const
{
  int err = pthread_cancel(handle);
  if (err)
    throw ThreadException("pthread_cancel", err);
}

void Thread::run_with_cancellation_cleanup(
    const std::function<void()>& code_routine, std::function<void()> cleanup_routine)
{
  pthread_cleanup_push(routine_wrapper_no_ret, reinterpret_cast<void*>(&cleanup_routine));
  code_routine();
  pthread_cleanup_pop(1);
}

void Thread::test_cancel() noexcept
{
  pthread_testcancel();
}

void Thread::yield()
{
  if (sched_yield()) {
    throw ThreadException("sched_yield", errno);
  }
}

void Thread::queue_signal(int sig, sigval value) const
{
  int err = pthread_sigqueue(handle, sig, value);
  if (err)
    throw ThreadException("pthread_sigqueue", err);
}

sigset_t Thread::set_signal_mask(int how, const sigset_t& set) const
{
  sigset_t old_set;
  int err = pthread_sigmask(how, &set, &old_set);
  if (err)
    throw ThreadException("pthread_sigmask", err);
  return old_set;
}

void Thread::set_concurrency_level(int level)
{
  int err = pthread_setconcurrency(level);
  if (err)
    throw ThreadException("pthread_setconcurrency", err);
}

int Thread::get_concurrency_level() noexcept
{
  return pthread_getconcurrency();
}

void Thread::kill(int sig) const
{
  int err = pthread_kill(handle, sig);
  if (err)
    throw ThreadException("pthread_kill", err);
}

const pthread_attr_t* ThreadAttributes::underlying_data() const noexcept
{
  return &attr;
}

#define ATTR_SETTER(EnumClass, pthread_function)                                                   \
  do {                                                                                             \
    int err = pthread_function(&attr, state);                                                      \
    if (err)                                                                                       \
      throw ThreadException(#pthread_function, err);                                               \
    return *this;                                                                                  \
  } while (0)

#define ATTR_GETTER(EnumClass, ParamType, pthread_function)                                        \
  do {                                                                                             \
    ParamType state;                                                                               \
    int err = pthread_function(&attr, &state);                                                     \
    if (err)                                                                                       \
      throw ThreadException(#pthread_function, err);                                               \
    return EnumClass(state);                                                                       \
  } while (0)

ThreadAttributes& ThreadAttributes::set_detach_state(AttrDetachState state)
{
  ATTR_SETTER(AttrDetachState, pthread_attr_setdetachstate);
}

AttrDetachState ThreadAttributes::get_detach_state() const
{
  ATTR_GETTER(AttrDetachState, int, pthread_attr_getdetachstate);
}

ThreadAttributes& ThreadAttributes::set_scope(AttrThreadScope state)
{
  ATTR_SETTER(AttrThreadScope, pthread_attr_setscope);
}

AttrThreadScope ThreadAttributes::get_scope() const
{
  ATTR_GETTER(AttrThreadScope, int, pthread_attr_getscope);
}

ThreadAttributes& ThreadAttributes::set_inherit_scheduler(AttrInheritScheduler state)
{
  ATTR_SETTER(AttrInheritScheduler, pthread_attr_setinheritsched);
}

AttrInheritScheduler ThreadAttributes::get_inherit_scheduler() const
{
  ATTR_GETTER(AttrInheritScheduler, int, pthread_attr_getinheritsched);
}

ThreadAttributes& ThreadAttributes::set_scheduling_policy(AttrSchedulingPolicy state)
{
  ATTR_SETTER(AttrSchedulingPolicy, pthread_attr_setschedpolicy);
}

AttrSchedulingPolicy ThreadAttributes::get_scheduling_policy() const
{
  ATTR_GETTER(AttrSchedulingPolicy, int, pthread_attr_getschedpolicy);
}

ThreadAttributes& ThreadAttributes::set_scheduling_priority(int prio)
{
  sched_param schedParam { prio };
  int err = pthread_attr_setschedparam(&attr, &schedParam);
  if (err)
    throw ThreadException("pthread_attr_setschedparam", err);
  return *this;
}

int ThreadAttributes::get_scheduling_priority() const
{
  sched_param state { 0 };
  int err = pthread_attr_getschedparam(&attr, &state);
  if (err)
    throw ThreadException("pthread_attr_getschedparam", err);
  return state.sched_priority;
}

ThreadAttributes& ThreadAttributes::set_guard_size(std::size_t state)
{
  ATTR_SETTER(std::size_t, pthread_attr_setguardsize);
}

std::size_t ThreadAttributes::get_guard_size() const
{
  ATTR_GETTER(std::size_t, std::size_t, pthread_attr_getguardsize);
}

ThreadAttributes& ThreadAttributes::set_stack_size(std::size_t state)
{
  ATTR_SETTER(std::size_t, pthread_attr_setstacksize);
}

std::size_t ThreadAttributes::get_stack_size() const
{
  ATTR_GETTER(std::size_t, std::size_t, pthread_attr_getstacksize);
}

ThreadAttributes& ThreadAttributes::set_stack_info(void* addr, std::size_t size)
{
  int err = pthread_attr_setstack(&attr, addr, size);
  if (err)
    throw ThreadException("pthread_attr_setstack", err);
  return *this;
}

StackInfo ThreadAttributes::get_stack_info() const
{
  StackInfo info { nullptr, 0 };
  int err = pthread_attr_getstack(&attr, &info.addr, &info.size);
  if (err)
    throw ThreadException("pthread_attr_getstack", err);
  return info;
}

#undef ATTR_SETTER
#undef ATTR_GETTER

SpinLock::~SpinLock()
{
  int err = pthread_spin_destroy(&lock);
  if (err)
    std::cerr << "(swallowed error in destructor) pthread_spin_destroy: " << err;
}

void SpinLock::spin_lock()
{
  int err = pthread_spin_lock(&lock);
  if (err)
    throw ThreadException("pthread_spin_lock", err);
}

void SpinLock::unlock()
{
  int err = pthread_spin_unlock(&lock);
  if (err)
    throw ThreadException("pthread_spin_unlock", err);
}

void SpinLock::try_lock()
{
  int err = pthread_spin_trylock(&lock);
  if (err)
    throw ThreadException("pthread_spin_trylock", err);
}

}  // namespace microloop
