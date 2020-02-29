//
// Copyright (c) 2020 by Victor Barbu. All Rights Reserved.
//

#include "uri/uri.h"

#include "absl/strings/str_join.h"
#include "uriparser/Uri.h"

namespace micro::uri::detail
{

static optional<string> from_range(UriTextRangeA range) noexcept
{
  if (!range.first | !range.afterLast)
  {
    return std::nullopt;
  }

  return std::string(range.first, range.afterLast);
}

static optional<vector<string>> from_path_list(UriPathSegmentA *head, UriPathSegmentA *tail)
{
  vector<string> path_segments;

  while (head)
  {
    path_segments.push_back(*from_range(head->text));
    head = head->next;
  }

  return path_segments;
}

optional<Uri> from_string(string str, bool normalized) noexcept
{
  UriUriA uri;
  if (uriParseSingleUriA(&uri, str.c_str(), nullptr) != URI_SUCCESS)
  {
    return std::nullopt;
  }

  if (normalized && uriNormalizeSyntaxA(&uri) != URI_SUCCESS)
  {
    uriFreeUriMembersA(&uri);
    return std::nullopt;
  }

  Uri wrapper{
    from_range(uri.scheme),
    from_range(uri.userInfo),
    from_range(uri.hostText),
    from_range(uri.portText),
    from_path_list(uri.pathHead, uri.pathTail),
    from_range(uri.query),
    from_range(uri.fragment),
    static_cast<bool>(uri.absolutePath)
  };

  uriFreeUriMembersA(&uri);

  return wrapper;
}

std::string Uri::path() const noexcept
{
  if (is_absolute_path())
  {
    return "/" + absl::StrJoin(*path_segments_, "/");
  }

  return absl::StrJoin(*path_segments_, "/");
}

}
