// Copyright 2019 Victor Barbu

#include <event_loop.h>
#include <event_sources/timeout.h>
#include <event_sources/filesystem/write.h>
#include <iostream>
#include <string>
#include <unistd.h>

void first_timer()
{
  std::cout << "First timer is done.\n";
}

void second_timer()
{
  std::cout << "Second timer is done.\n";
}

int main()
{
  std::string buffer(471859200, 'v');
  MICROLOOP_FS_WRITE("/tmp/test.test", buffer, [](ssize_t s) { std::cout << "Done writing " << s << " bytes.\n"; });

  MICROLOOP_SET_TIMEOUT(first_timer, 4500);
  MICROLOOP_SET_TIMEOUT(second_timer, 2000);
  MICROLOOP_SET_TIMEOUT([]() { std::cout << "Third timer is done.\n"; }, 100);

  while (true) {
    MICROLOOP_TICK();
  }
}
