//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "event_source.h"

#include <cstdint>
#include <functional>
#include <map>
#include <vector>

namespace microloop
{

class SignalsMonitor : public EventSource
{
public:
  /**
   * Callbacks to be called when signals are received by the application. The callback receives the
   * signal number as parameter. The callback should return true if, from its perspective the
   * application can end after its execution. If the application is not considered ready to be
   * closed after the signal is received then false should be returned.
   */
  using SignalHandler = std::function<bool(std::uint32_t signal)>;

  /**
   * Create an instance of the Signal Monitor. Upon construction, the signal monitor will not block
   * any pending signal.
   */
  SignalsMonitor();

  ~SignalsMonitor();

  void start() override
  {}

  /**
   * Read pending signals and trigger callbacks for the registered blocked signals.
   */
  void run_callback() override;

  virtual std::uint32_t produced_events() const override
  {
    return EPOLLIN;
  }

  /**
   * Register a new handler for the given signal. If a handler for the given signal already exists,
   * the new one will just be appended to the list of handlers for that signal.
   * @param sig Signal to trigger the given handler. This will be blocked by the process sigmask.
   * @param handler The callable to be invoked when the given signal is caught.
   */
  void register_signal_handler(std::uint32_t sig, SignalHandler &&handler);

  /**
   * @return The current process sigmask.
   */
  const sigset_t &get_sigmask() const
  {
    return curr_sigset;
  }

private:
  /**
   * Block or unblock the given signal.
   * @param how One of SIG_BLOCK/SIG_UNBLOCK constants.
   * @param sig The number of the signal to be blocked/unblocked.
   */
  void update_sigmask(int how, int sig);

  sigset_t initial_sigset;
  sigset_t curr_sigset;
  std::map<std::uint32_t, std::vector<SignalHandler>> signal_handlers;
};

}  // namespace microloop
