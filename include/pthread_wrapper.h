// Copyright 2019 Stefan Silviu-Alexandru

#ifndef MICROLOOP_PTHREAD_WRAPPER_H
#define MICROLOOP_PTHREAD_WRAPPER_H

#include <pthread.h>
#include <stdexcept>
#include <functional>
#include <type_traits>
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

enum AttrDetachState : int {
  DETACHED = PTHREAD_CREATE_DETACHED,
  JOINABLE = PTHREAD_CREATE_JOINABLE
};

enum AttrThreadScope : int {
  SYSTEM = PTHREAD_SCOPE_SYSTEM,
  PROCESS = PTHREAD_SCOPE_PROCESS /// Unsupported operation on Linux
};

enum AttrInheritScheduler : int {
  INHERIT = PTHREAD_INHERIT_SCHED,
  EXPLICIT = PTHREAD_EXPLICIT_SCHED
};

enum AttrSchedulingPolicy : int {
  FIFO = SCHED_FIFO,
  ROUND_ROBIN = SCHED_RR,
  OTHER = SCHED_OTHER /// Default Linux time-sharing scheduling
};

struct StackInfo {
  void* addr;
  std::size_t size;
};

/**
 * Builder wrapper for @ref pthread_attr_t.
 * @sa PThread
 */
class PThreadAttributes {
  pthread_attr_t attr;
public:
  explicit PThreadAttributes();
  ~PThreadAttributes();
  PThreadAttributes(const PThreadAttributes&) = default;
  PThreadAttributes& operator=(const PThreadAttributes&) = default;

  const pthread_attr_t* getUnderlyingData() const noexcept;

  PThreadAttributes& setDetachState(AttrDetachState);
  AttrDetachState getDetachState() const;

  PThreadAttributes& setScope(AttrThreadScope);
  AttrThreadScope getScope() const;

  PThreadAttributes& setInheritScheduler(AttrInheritScheduler);
  AttrInheritScheduler getInheritScheduler() const;

  PThreadAttributes& setSchedulingPolicy(AttrSchedulingPolicy);
  AttrSchedulingPolicy getSchedulingPolicy() const;

  PThreadAttributes& setSchedulingPriority(int);
  int getSchedulingPriority() const;

  PThreadAttributes& setGuardSize(std::size_t);
  std::size_t getGuardSize() const;

  PThreadAttributes& setStackSize(std::size_t);
  std::size_t getStackSize() const;

  PThreadAttributes& setStackInfo(void*, std::size_t);
  StackInfo getStackInfo() const;
};

using ThreadRoutine = void*(void*);

/**
 * High-level representation of a thread handle. Implements various common thread operations.
 */
class PThread {
  pthread_t handle;
  bool detached = false;

  /** Used for making shallow copies in @ref self. */
  explicit PThread(pthread_t) noexcept;

  /**
   * Internal constructor for calling @ref pthread_create.
   * @param targetRoutine function to run on new thread
   * @param routineData data to pass to targetRoutine
   * @param attrs attributes for the new thread
   */
  PThread(ThreadRoutine targetRoutine, void* routineData, const pthread_attr_t* attrs);

  /**
   * Internal constructor for isolating usage of reinterpret_cast.
   * @tparam Functor highLevelRoutine's type
   * @param carrierRoutine a Functor-specific wrapper for bootstrapping the highLevelRoutine
   * @param highLevelRoutine the actual code to run on the new thread
   * @param attrs attributes for the new thread
   */
  template<
      typename Functor,
      typename std::enable_if_t<
          !std::is_same_v<std::nullptr_t, Functor> && std::is_invocable_r<void*, Functor, void*>::value> = 0>
  PThread(ThreadRoutine carrierRoutine, Functor highLevelRoutine, const pthread_attr_t* attrs) :
      PThread(carrierRoutine, reinterpret_cast<void*>(highLevelRoutine), attrs) {}

public:
  /** Create a thread and run the specified routine. */
  explicit PThread(ThreadRoutine);
  /** Create a thread with the given attributes and run the specified routine. */
  PThread(ThreadRoutine, const PThreadAttributes&);

  /** Create a thread and run the specified routine. */
  explicit PThread(std::function<void()>);
  /** Create a thread with the given attributes and run the specified routine. */
  PThread(std::function<void()>, const PThreadAttributes&);

  /** Create a thread and run the specified routine that returns a value. */
  static PThread makeWithRet(std::function<void*()>);
  /** Create a thread with the given attributes and run the specified routine that returns a value. */
  static PThread makeWithRet(std::function<void*()>, const PThreadAttributes&);

  /** Returns the @ref PThread object for the current thread. */
  static PThread self() noexcept;
  /** Wrapper for @ref pthread_exit. */
  static noreturn void exit(void* retVal = nullptr) noexcept;

  /**
   * Safer wrapper for @ref pthread_join.
   * @tparam T cast the thread's return value to a pointer of this type
   */
  template<typename T = void>
  T* join() {
    void* retValPtr;
    int err = pthread_join(handle, &retValPtr);
    if (err) throw PThreadException("pthread_join", err);
    return static_cast<T*>(retValPtr);
  }

  /** Detaches this thread after construction. */
  void detach();

  bool operator==(const PThread& other) const noexcept;
  bool operator!=(const PThread& other) const noexcept;
};

}

#endif // MICROLOOP_PTHREAD_WRAPPER_H
