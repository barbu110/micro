//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/buffer.h"
#include "uri/uri.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
  auto uri_optional = micro::uri::from_string(std::string{argv[1]});
  if (!uri_optional)
  {
    std::cerr << "invalid uri\n";
    return -1;
  }

  auto uri = *uri_optional;

  if (uri.has_scheme())
  {
    std::cout << "scheme = " << uri.scheme() << std::endl;
  }
  if (uri.has_user_info())
  {
    std::cout << "user_info = " << uri.user_info() << std::endl;
  }
  if (uri.has_host_text())
  {
    std::cout << "host_text = " << uri.host_text() << std::endl;
  }
  if (uri.has_port_text())
  {
    std::cout << "port_text = " << uri.port_text() << std::endl;
  }
  if (uri.has_path())
  {
    std::cout << "path = " << uri.path() << std::endl;
  }
  if (uri.has_query())
  {
    std::cout << "query = " << uri.query() << std::endl;
  }
  if (uri.has_fragment())
  {
    std::cout << "fragment = " << uri.fragment() << std::endl;
  }

  return 0;
}
