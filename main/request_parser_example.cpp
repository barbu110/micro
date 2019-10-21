//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/request_parser.h"

#include <vector>
#include <string>
#include <iostream>

int main()
{
  using std::cout;
  using std::endl;

  std::vector<const char *> chunks = {
    "POST /foo HTTP/1.1\r\n",
    "Content-Length: 7\r\n\r\n",
    "example"
  };

  microhttp::http::RequestParser parser{};

  for (const auto &chunk : chunks)
  {
    cout << "Chunk: " << chunk << endl;

    parser.add_chunk(chunk);
  }

  auto &request = parser.get_parsed_request();

  cout << "HTTP Version: " << static_cast<std::string>(request.get_http_version()) << endl;
  cout << "Method: " << request.get_http_method() << endl;
  cout << "Request Target: " << request.get_uri() << endl;

  auto count = request.get_headers().size();
  cout << endl << "Request Headers [count: " << count << "]\n";

  for (const auto &header : request.get_headers())
  {
    std::cout << header.first << ": " << header.second << "[END]\n";
  }

  cout << endl << "Body [size: " << request.get_body().size() << "]\n";

  cout << request.get_body().str() << "[END]\n";

  return 0;
}
