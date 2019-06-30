//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <functional>
#include <tuple>

namespace microloop::detail {

template <class... Types> class EventSourceHelper {
public:
  using Result = std::tuple<Types...>;
  using Callback = std::function<void(Types...)>;
};

}  // namespace microloop::detail
