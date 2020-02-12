//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/buffer.h"
#include "microhttp/uri.h"

#include <iostream>
#include <string>

int main()
{
  std::cout << "Parsing..." << std::endl;

  std::string uri{"ssh://victor%20barbu@[v8.6234bvw]:3306"};
  microhttp::http::UriParser parser{};

  auto parsed = parser.parse(uri);

  if (!parsed)
  {
    std::cerr << "Failed with error: " << parser.error() << std::endl;
    return 1;
  }

  std::cout << "Scheme: " << parsed->scheme << std::endl;
  std::cout << "User information: " << parsed->userinfo << std::endl;
  std::cout << "Host: " << parsed->host << std::endl;
  std::cout << "Port: " << parsed->port << std::endl;


  return 0;
}
