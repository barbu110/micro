//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <functional>
#include <tuple>

namespace microloop {

class EventLoop;

template <class... ReturnTypeParams>
class TypeHelper {
public:
  using ReturnType = std::tuple<ReturnTypeParams...>;
  using Callback = std::function<void(ReturnTypeParams...)>;
};

class EventSource {
  friend class EventLoop;

public:
  virtual ~EventSource()
  {}

protected:
  int get_id() const
  {
    return id;
  }

  void set_id(int id)
  {
    this->id = id;
  }

  virtual bool has_fd() const
  {
    return true;
  }

  virtual void start()
  {}

  virtual void run_callback()
  {}

private:
  int id;
};

}  // namespace microloop
