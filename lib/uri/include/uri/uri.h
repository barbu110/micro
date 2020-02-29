//
// Copyright (c) 2020 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include <optional>
#include <string>
#include <vector>


#define COMPONENT_F(type, name)                                                                    \
  bool has_##name() const noexcept                                                                 \
  {                                                                                                \
    return static_cast<bool>(name##_);                                                             \
  }                                                                                                \
  type name() const noexcept                                                                       \
  {                                                                                                \
    return *name##_;                                                                               \
  }

#define COMPONENT(type, name) std::optional<type> name##_


namespace micro::uri
{

namespace detail
{

  using std::optional;
  using std::string;
  using std::vector;

  class Uri
  {

  public:
    Uri(optional<string> scheme, optional<string> user_info, optional<string> host_text,
        optional<string> port_text, optional<vector<string>> path_segments, optional<string> query,
        optional<string> fragment, bool absolute_path) :
        scheme_{scheme},
        user_info_{user_info},
        host_text_{host_text},
        port_text_{port_text},
        path_segments_{path_segments},
        query_{query},
        fragment_{fragment},
        absolute_path_{absolute_path}
    {}

    COMPONENT_F(string, scheme)
    COMPONENT_F(string, user_info)
    COMPONENT_F(string, host_text)
    // TODO Add host_data
    COMPONENT_F(string, port_text)
    COMPONENT_F(string, query)
    COMPONENT_F(string, fragment)

    bool has_path() const noexcept
    {
      return static_cast<bool>(path_segments_);
    }

    vector<string> path_segments() const noexcept
    {
      return *path_segments_;
    }

    string path() const noexcept;

    bool is_absolute_path() const noexcept
    {
      return absolute_path_;
    }

  private:
    COMPONENT(string, scheme);
    COMPONENT(string, user_info);
    COMPONENT(string, host_text);
    COMPONENT(string, port_text);
    COMPONENT(string, query);
    COMPONENT(string, fragment);
    COMPONENT(vector<string>, path_segments);

    bool absolute_path_;
  };

  std::optional<Uri> from_string(std::string str, bool normalized = true) noexcept;

}  // namespace detail


using detail::from_string;
using detail::Uri;

}  // namespace micro::uri


#undef COMPONENT_F
#undef COMPONENT
