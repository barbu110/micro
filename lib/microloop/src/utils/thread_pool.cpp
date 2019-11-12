//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/utils/thread_pool.h"

#include <pthread.h>
#include <signal.h>
#include <cstdlib>
#include <charconv>

namespace microloop::utils
{

ThreadPool::ThreadPool()
{
  std::uint32_t threads_count = 0;
  if (const char *val = std::getenv(MAX_WORKERS_ENV_VAR); val != nullptr)
  {
    std::from_chars(val, val + std::strlen(val) + 1, threads_count);
  }

  if (!threads_count)
  {
    threads_count = THREADPOOL_MAX_WORKERS;
  }
  else
  {
    threads_count = std::min(threads_count, std::thread::hardware_concurrency() - 1);
  }

  try
  {
    for (std::uint8_t i = 0; i != threads_count; ++i)
    {
      threads.emplace_back(&ThreadPool::worker, this);
    }
  }
  catch (...)
  {
    destroy();

    throw;
  }
}

void ThreadPool::worker()
{
  /*
   * We block all signals on the worker threads. Signals will only be received by the rooot thread
   * in the signalfd mechanism.
   */
  sigset_t mask;
  sigfillset(&mask);
  if (pthread_sigmask(SIG_SETMASK, &mask, nullptr) != 0)
  {
    throw microloop::KernelException(errno);
  }

  while (!done_)
  {
    auto &&curr_job = jobs.wait_pop();
    if (curr_job.has_value())
    {
      /*
       * Note that we dereference the std::optional first so we can be able to access the
       * std::unique_ptr contained within.
       */
      (*curr_job)->run();
    }
  }
}

void ThreadPool::destroy()
{
  done_ = true;
  jobs.invalidate();

  for (auto &thread : threads)
  {
    if (thread.joinable())
    {
      thread.join();
    }
  }

  threads.clear();
}

}  // namespace microloop::utils
