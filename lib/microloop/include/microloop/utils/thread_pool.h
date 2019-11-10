//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microloop/kernel_exception.h"

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

    Job(const Job &) = delete;
    Job &operator=(const Job &) = delete;

    Job(Func &&fn) : fn{fn}
    {}

    void run()
    {
      fn();
    }

    virtual ~Job()
    {
      /*
       * This virtual destructor has been added in order for Valgrind not to complain.
       * TODO Find out why it works, although there is no class inheriting from Job.
       */
    }

  private:
    Func fn;
  };

  template <class JobType>
  class JobsQueue
  {
  public:
    ~JobsQueue()
    {
      invalidate();
    }

    std::optional<JobType> try_pop()
    {
      std::lock_guard<std::mutex> lock{mutex_};
      if (queue_.empty() || !valid_)
      {
        return std::nullopt;
      }

      return std::move(queue_.front());
      queue_.pop();
    }

    std::optional<JobType> wait_pop()
    {
      std::unique_lock<std::mutex> lock{mutex_};
      condition_.wait(lock, [&] { return !queue_.empty() || !valid_; });

      if (!valid_)
      {
        return std::nullopt;
      }

      auto elem = std::move(queue_.front());
      queue_.pop();

      return std::make_optional(std::move(elem));
    }

    void push(JobType &&job)
    {
      std::lock_guard<std::mutex> lock{mutex_};
      queue_.push(std::move(job));
      condition_.notify_one();
    }

    bool empty() const
    {
      std::lock_guard<std::mutex> lock{mutex_};
      return queue_.empty();
    }

    void clear()
    {
      std::lock_guard<std::mutex> lock{mutex_};
      while (!queue_.empty())
      {
        queue_.pop();
      }

      condition_.notify_all();
    }

    void invalidate()
    {
      std::lock_guard<std::mutex> lock{mutex_};
      valid_ = false;
      condition_.notify_all();
    }

    bool valid() const
    {
      std::lock_guard<std::mutex> lock{mutex_};
      return valid_;
    }

  private:
    std::atomic_bool valid_{true};
    mutable std::mutex mutex_;
    std::queue<JobType> queue_;
    std::condition_variable condition_;
  };

public:
  /**
   * Constructs the thread pool with the given number of threads.
   * @param threads_count How many threads to spawn. If this number exceeds the number of physical
   * threads available on the system minus one, then it's set to that. A minimum of one thread is
   * spawned.
   */
  ThreadPool(std::uint32_t threads_count);

  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool &operator=(const ThreadPool &other) = delete;

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
    jobs.push(std::make_unique<Job>(std::move(bound_fn)));
  }

  /**
   * Close the thread pool.
   */
  void close()
  {
    destroy();
  }

  ~ThreadPool()
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
  std::vector<std::thread> threads{};

  /**
   * Jobs to be executed by worker threads.
   */
  JobsQueue<std::unique_ptr<Job>> jobs{};

  std::atomic_bool done_ = false;
};

}  // namespace microloop::utils
