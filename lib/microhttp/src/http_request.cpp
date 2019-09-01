//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/http_request.h"

namespace microhttp::http
{

HttpRequest::HttpRequest() : http_version{1, 1}
{}

HttpRequest::HttpRequest(std::string http_method, std::string uri) :
  http_version{1, 1}, http_method{http_method}, uri{uri}
{}

}
