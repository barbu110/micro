//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microloop/buffer.h"
#include "microhttp/status_codes.h"

namespace microhttp::http
{

class ResponseWriter
{
public:
  ResponseWriter();

private:
  /**
   * The HTTP status code associated with the response.
   */
  StatusCode status_code;
};

}  // namespace microhttp::http
