// Copyright 2019 Stefan Silviu-Alexandru

#include <unistd.h>
#include <pthread_wrapper.h>
#include <iostream>

using namespace microloop;

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
  PThread thread1(thread1Routine), thread2(thread2Routine);
  thread1.detach();

  PThread thread3([]() {
    sleep(3);
    std::cout << "Hello thread 3" << std::endl;
  });

  int test = 42;

  PThread thread4 = PThread::makeWithRet([&]() {
    std::cout << "Hello thread 4" << std::endl;
    return &test;
  });

  auto ptrToTest = thread4.join<int>();
  thread2.join();
  thread3.join();

  std::cout << "Thread 4 returned " << *ptrToTest << std::endl;
  return 0;
}
