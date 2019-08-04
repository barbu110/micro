//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "kernel_exception.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace microloop::utils
{

class ThreadPool
{
  class Job
  {
  public:
    using Func = std::function<void()>;

    Job(Func &&fn) : fn{fn}
    {}

    void run()
    {
      fn();
    }

  private:
    Func fn;
  };

public:
  /**
   * Constructs the thread pool with the given number of threads.
   * @param threads_count How many threads to spawn. If this number exceeds the number of physical
   * threads available on the system minus one, then it's set to that. A minimum of one thread is
   * spawned.
   */
  ThreadPool(std::uint32_t threads_count);

  /**
   * Submit a new job to the thread pool. Note that the thread pool is not responsible for
   * processing the results of the job function.
   * @param fn The function representing the job.
   * @param args Arguments to pass to the given function.
   */
  template <class Func, class... Args>
  void submit(Func &&fn, Args &&... args)
  {
    auto bound_fn = std::bind(std::forward<Func>(fn), std::forward<Args>(args)...);

    Job job{std::move(bound_fn)};

    std::unique_lock<std::mutex> lock{mtx};
    jobs.push(std::make_unique<Job>(std::move(job)));

    lock.unlock();
    cond.notify_one();
  }

  /**
   * Close the thread pool.
   */
  void close()
  {
    destroy();
  }

private:
  /**
   * The thread worker. This function is responsible for:
   *  1) Blocking all the signals on the thread it represents. (This is required in the context of
   *     the SignalsMonitor)
   *  2) Waiting for new jobs and completing them when woke up.
   */
  void worker();

  /**
   * Destroys the thread pool (closing all the threads and clearing jobs).
   */
  void destroy();

  /**
   * Vector of created threads.
   */
  std::vector<std::thread> threads;

  /**
   * Whether the threadpool is done or not. When this is true, the threads are automatically
   * closed.
   */
  std::atomic_bool done;

  /**
   * Jobs to be executed by worker threads.
   */
  std::queue<std::unique_ptr<Job>> jobs;

  mutable std::mutex mtx;
  std::condition_variable cond;
};

}  // namespace microloop::utils
