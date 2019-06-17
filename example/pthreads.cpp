// Copyright 2019 Stefan Silviu-Alexandru

#include <unistd.h>
#include <pthread_wrapper.h>
#include <iostream>

void* thread1Routine(void*) {
  sleep(2);
  std::cout << "Hello thread 1" << std::endl;
  return nullptr;
}

void* thread2Routine(void*) {
  sleep(1);
  std::cout << "Hello thread 2" << std::endl;
  return nullptr;
}

int main() {
  using namespace microloop;
  PThread thread1(thread1Routine), thread2(thread2Routine);

  PThread thread3([]() {
    sleep(3);
    std::cout << "Hello thread 3" << std::endl;
  });

  PThread thread4 = PThread::makeWithRet([]() {
    std::cout << "Hello thread 4" << std::endl;
    return nullptr;
  });

  thread3.join();
  return 0;
}
