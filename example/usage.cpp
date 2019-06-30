// Copyright 2019 Victor Barbu

#include <iostream>
#include <string>
#include <microloop.h>

void first_timer()
{
  std::cout << "First timer is done.\n";
}

void second_timer()
{
  std::cout << "Second timer is done.\n";
}

void on_read(std::string data)
{
  std::cout << "Read: " << data.size() << " bytes.\n";
  std::cout << data << "\n";
}

using namespace microloop;

int main()
{
  std::string buffer(471859200, 'v');
  fs::write("/tmp/test.test", buffer, [](ssize_t s) {
    std::cout << "Done writing " << s << " bytes.\n";
  });

  timers::set_timeout(first_timer, 4500);
  timers::set_timeout(second_timer, 2000);
  timers::set_timeout([]() { std::cout << "Third timer is done.\n"; }, 100);

  fs::read("data.txt", on_read);

  while (true) {
    MICROLOOP_TICK();
  }
}
