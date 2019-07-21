//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <ctime>
#include <iostream>

namespace microloop::utils
{

#if defined(DEBUG)

template <class Arg>
static void LogDebug(const Arg &arg)
{
  std::cout << arg << std::endl;
}

template <class Arg, class... Rest>
static void LogDebug(const Arg &arg, const Rest &... rest)
{
  std::cout << arg << " ";
  LogDebug(rest...);
}

#else

template <class... Args>
static void LogDebug(const Args &...)
{}

#endif

}  // namespace microloop::utils
