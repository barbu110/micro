// Copyright 2019 Stefan Silviu-Alexandru

#pragma once

#include <cstring>
#include <functional>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <stdexcept>
#include <type_traits>

#define noreturn __attribute((noreturn))

namespace microloop {

class ThreadException : public std::runtime_error {
public:
  const int error;

  explicit ThreadException(const std::string& source, int error) :
      std::runtime_error(source + " error: " + strerror(error)), error(error)
  {}
};

enum AttrDetachState : int
{
  DETACHED = PTHREAD_CREATE_DETACHED,
  JOINABLE = PTHREAD_CREATE_JOINABLE
};

enum AttrThreadScope : int
{
  SYSTEM = PTHREAD_SCOPE_SYSTEM,
  PROCESS = PTHREAD_SCOPE_PROCESS  /// Unsupported operation on Linux
};

enum AttrInheritScheduler : int
{
  INHERIT = PTHREAD_INHERIT_SCHED,
  EXPLICIT = PTHREAD_EXPLICIT_SCHED
};

enum AttrSchedulingPolicy : int
{
  FIFO = SCHED_FIFO,
  ROUND_ROBIN = SCHED_RR,
  OTHER = SCHED_OTHER  /// Default Linux time-sharing scheduling
};

struct StackInfo {
  void* addr;
  std::size_t size;
};

/**
 * Builder wrapper for @ref pthread_attr_t.
 * @sa PThread
 */
class ThreadAttributes {
  pthread_attr_t attr;

public:
  explicit ThreadAttributes();
  ~ThreadAttributes();
  ThreadAttributes(const ThreadAttributes&) = default;
  ThreadAttributes& operator=(const ThreadAttributes&) = default;

  const pthread_attr_t* underlying_data() const noexcept;

  ThreadAttributes& set_detach_state(AttrDetachState);
  AttrDetachState get_detach_state() const;
  ThreadAttributes& set_scope(AttrThreadScope);
  AttrThreadScope get_scope() const;
  ThreadAttributes& set_inherit_scheduler(AttrInheritScheduler);
  AttrInheritScheduler get_inherit_scheduler() const;
  ThreadAttributes& set_scheduling_policy(AttrSchedulingPolicy);
  AttrSchedulingPolicy get_scheduling_policy() const;
  ThreadAttributes& set_scheduling_priority(int);
  int get_scheduling_priority() const;
  ThreadAttributes& set_guard_size(std::size_t);
  std::size_t get_guard_size() const;
  ThreadAttributes& set_stack_size(std::size_t);
  std::size_t get_stack_size() const;
  ThreadAttributes& set_stack_info(void* addr, std::size_t);
  StackInfo get_stack_info() const;
};

enum ThreadCancelState : int
{
  ENABLED = PTHREAD_CANCEL_ENABLE,
  DISABLED = PTHREAD_CANCEL_DISABLE
};

enum ThreadCancelType : int
{
  DEFERRED = PTHREAD_CANCEL_DEFERRED,
  ASYNCHRONOUS = PTHREAD_CANCEL_ASYNCHRONOUS
};

using ThreadRoutine = void*(void*);

/**
 * High-level representation of a thread handle. Implements various common thread operations.
 */
class Thread {
  pthread_t handle;
  bool detached = false;

  /** Used for making shallow copies in self. */
  explicit Thread(pthread_t) noexcept;

  /**
   * Internal constructor for calling pthread_create.
   * @param target_routine function to run on new thread
   * @param routine_data data to pass to target_routine
   * @param attrs attributes for the new thread
   */
  Thread(ThreadRoutine target_routine, void* routine_data, const pthread_attr_t* attrs);

  /**
   * Internal constructor for isolating usage of reinterpret_cast.
   * @tparam Functor high_level_routine's type
   * @param carrier_routine a Functor-specific wrapper for bootstrapping the high_level_routine
   * @param high_level_routine the actual code to run on the new thread
   * @param attrs attributes for the new thread
   */
  template <typename Functor,
      typename std::enable_if_t<
          !std::is_same_v<std::nullptr_t,
              Functor> && std::is_invocable_r<void*, Functor, void*>::value> = 0>
  Thread(ThreadRoutine carrier_routine, Functor high_level_routine, const pthread_attr_t* attrs) :
      Thread(carrier_routine, reinterpret_cast<void*>(high_level_routine), attrs)
  {}

public:
  /** Create a thread and run the specified routine. */
  explicit Thread(ThreadRoutine);

  /** Create a thread with the given attributes and run the specified routine. */
  Thread(ThreadRoutine, const ThreadAttributes&);

  /** Create a thread and run the specified routine. */
  explicit Thread(std::function<void()>);

  /** Create a thread with the given attributes and run the specified routine. */
  Thread(std::function<void()>, const ThreadAttributes&);

  /** Create a thread and run the specified routine that returns a value. */
  static Thread make_with_ret(std::function<void*()> routine);

  /** Create a thread with the given attributes and run the specified routine that returns a value.
   */
  static Thread make_with_ret(std::function<void*()> routine, const ThreadAttributes& attrs);

  /** Returns the @ref PThread object for the current thread. */
  static Thread self() noexcept;

  /** Wrapper for @ref pthread_exit. */
  static noreturn void exit(void* ret_val = nullptr) noexcept;

  /**
   * Have the current thread yield the processor. Intended for use with @ref
   * AttrSchedulingPolicy.FIFO or
   * @ref AttrSchedulingPolicy.ROUND_ROBIN.
   *
   * @ref pthread_yield is non-portable, but @ref sched_yield is, and the former is implemented
   * using the latter on at least Linux.
   */
  static void yield();

  /** Gets and sets the cancellation state for the current thread. */
  static ThreadCancelState set_cancel_state(ThreadCancelState);

  /** Gets and sets the cancellation type for the current thread. */
  static ThreadCancelType set_cancel_type(ThreadCancelType);

  /**
   * Run code with a cleanup routine pushed on the stack.
   * @param code_routine the code to run on the cancellation area
   * @param cleanup_routine the actual cleanup code for @ref pthread_cleanup_push
   * @sa pthread_cleanup_pop
   */
  static void run_with_cancellation_cleanup(
      const std::function<void()>& code_routine, std::function<void()> cleanup_routine);

  /** @sa pthread_testcancel */
  static void test_cancel() noexcept;

  /** @sa pthread_setconcurrency */
  static void set_concurrency_level(int);

  /** @sa pthread_getconcurrency */
  static int get_concurrency_level() noexcept;

  /**
   * Safer wrapper for @ref pthread_join.
   * @tparam T cast the thread's return value to a pointer of this type
   */
  template <typename T = void> T* join()
  {
    void* retValPtr;
    int err = pthread_join(handle, &retValPtr);
    if (err)
      throw ThreadException("pthread_join", err);
    return static_cast<T*>(retValPtr);
  }

  /** Detaches this thread after construction. */
  void detach();

  /** Queue a cancellation request to this thread. */
  void cancel() const;

  /**
   * Queue a signal to this thread.
   * @sa pthread_sigqueue
   */
  void queue_signal(int sig, union sigval value) const;

  /** Get and set this thread's signal mask. */
  sigset_t set_signal_mask(int how, const sigset_t& set) const;

  /** Send the specified signal to this thread. */
  void kill(int sig) const;

  bool operator==(const Thread& other) const noexcept;

  bool operator!=(const Thread& other) const noexcept;
};

enum LockShareState : int
{
  PROCESS_PRIVATE = PTHREAD_PROCESS_PRIVATE,
  PROCESS_SHARED = PTHREAD_PROCESS_SHARED
};

class SpinLock {
  pthread_spinlock_t lock;

public:
  explicit SpinLock(LockShareState);

  ~SpinLock();

  SpinLock(const SpinLock&) = default;

  SpinLock& operator=(const SpinLock&) = default;

  void spin_lock();

  void unlock();

  void try_lock();
};

}  // namespace microloop
