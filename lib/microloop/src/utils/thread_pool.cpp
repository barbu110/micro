//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/utils/thread_pool.h"

#include <pthread.h>
#include <signal.h>

namespace microloop::utils
{

ThreadPool::ThreadPool(std::uint32_t threads_count)
{
  if (!threads_count)
  {
    throw std::invalid_argument("threads_count must be at least 1");
  }

  threads_count = std::min(std::thread::hardware_concurrency() - 1, threads_count);

  try
  {
    for (std::uint8_t i = 0; i != threads_count; ++i)
    {
      threads.emplace_back(&ThreadPool::worker, this);
      threads.back().detach();
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
