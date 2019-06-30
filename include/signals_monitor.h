//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <event_source.h>

namespace microloop {

class SignalsMonitor : public EventSource {
public:
  SignalsMonitor();
  ~SignalsMonitor();
  EventSource::TrackingData get_tracking_data() const override;

  void start() override
  {}

  void cleanup() override
  {}

  void notify() override
  {}

private:
  int fd;
};

}
