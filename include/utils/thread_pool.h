//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <signal.h>

namespace microloop::utils {

class ThreadPool {
  class Job {
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
  ThreadPool(std::uint32_t threads_count) : done{false}
  {
    if (!threads_count) {
      throw std::invalid_argument("threads_count must be at least 1");
    }

    threads_count = std::min(std::thread::hardware_concurrency() - 1, threads_count);

    try {
      for (std::uint8_t i = 0; i != threads_count; ++i) {
        threads.emplace_back(&ThreadPool::worker, this);
        threads.back().detach();
      }
    } catch (...) {
      destroy();

      throw;
    }
  }

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

  void close()
  {
    destroy();
  }

private:
  void worker()
  {
    /*
     * We block all signals on the worker threads. Signals will only be received by the rooot thread
     * in the signalfd mechanism.
     */
    sigset_t mask;
    sigfillset(&mask);
    if (pthread_sigmask(SIG_SETMASK, &mask, nullptr) != 0) {
      throw microloop::KernelException(errno);
    }

    while (!done) {
      std::unique_lock<std::mutex> lock{mtx};
      cond.wait(lock, [&] { return !jobs.empty(); });

      auto curr_job = std::move(jobs.front());
      jobs.pop();

      lock.unlock();
      cond.notify_one();

      curr_job->run();
    }
  }

  void destroy()
  {
    done = true;

    std::unique_lock<std::mutex> lock{mtx};
    while (!jobs.empty()) {
      jobs.pop();
    }

    lock.unlock();
    cond.notify_all();

    for (auto &thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  std::vector<std::thread> threads;

  /**
   * Whether the threadpool is done or not. When this is true, the threads are automatically
   * closed.
   */
  std::atomic_bool done;
  std::queue<std::unique_ptr<Job>> jobs;
  mutable std::mutex mtx;
  std::condition_variable cond;
};

}  // namespace microloop::utils
