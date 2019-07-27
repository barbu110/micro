//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "utils/thread_pool.h"

namespace microloop::utils
{

ThreadPool::ThreadPool(std::uint32_t threads_count) : done{false}
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

  while (!done)
  {
    std::unique_lock<std::mutex> lock{mtx};
    cond.wait(lock, [&] { return !jobs.empty(); });

    auto curr_job = std::move(jobs.front());
    jobs.pop();

    lock.unlock();
    cond.notify_one();

    curr_job->run();
  }
}

void ThreadPool::destroy()
{
  done = true;

  std::unique_lock<std::mutex> lock{mtx};
  while (!jobs.empty())
  {
    jobs.pop();
  }

  lock.unlock();
  cond.notify_all();

  for (auto &thread : threads)
  {
    if (thread.joinable())
    {
      thread.join();
    }
  }
}

}  // namespace microloop::utils
