//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma oonce

#include <map>
#include <cstdint>

namespace microloop {

class EventSource;

class EventSourcesTable {
public:
  enum LookupType {
    FD,
    THREAD_ID,
  };

  void set(LookupType lookup_type, std::int32_t id, EventSource *value)
  {
    data[get_key(lookup_type, id)] = value;
  }

  EventSource *get(LookupType lookup_type, std::int32_t id)
  {
    return data[get_key(lookup_type, id)];
  }

  void delete(LookupType lookup_type, std::int32_t id)
  {
    data.erase(get_key(lookup_type, id));
  }

private:
  inline std::uint64_t get_key(LookupType lookup_type, std::int32_t id) const
  {
    switch (lookup_type) {
    case FD:
      return id;
    case THREAD_ID:
      return id << 4;
    }
  }

  std::map<std::uint64_t, EventSource *> data;
};

}
