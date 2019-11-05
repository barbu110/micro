//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microloop/buffer.h"

#include <iostream>

int main()
{
  microloop::Buffer a{"foo"};
  microloop::Buffer b{a};

  std::cout << a.str_view() << "\n" << b.str_view() << "\n";

  return 0;
}
