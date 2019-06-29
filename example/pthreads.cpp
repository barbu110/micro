// Copyright 2019 Stefan Silviu-Alexandru

#include <cassert>
#include <iostream>
#include <pthread_wrapper.h>
#include <unistd.h>

using namespace microloop;

void* thread1Routine(void*)
{
  sleep(2);
  std::cout << "Hello thread 1" << std::endl;
  return nullptr;
}

void* thread2Routine(void*)
{
  sleep(1);
  std::cout << "Hello thread 2" << std::endl;
  return nullptr;
}

int main()
{
  PThread thread1(thread1Routine);
  thread1.detach();

  PThread thread2(thread2Routine, PThreadAttributes().setDetachState(JOINABLE).setScope(SYSTEM).setInheritScheduler(INHERIT).setSchedulingPolicy(OTHER).setSchedulingPriority(0).setGuardSize(4096).setStackSize(0x201000));

  PThread thread3([]() {
    sleep(3);
    std::cout << "Hello thread 3" << std::endl;
  });

  int test = 42;

  PThread thread4 = PThread::makeWithRet([&]() {
    std::cout << "Hello thread 4" << std::endl;
    return &test;
  });

  PThread thread5 = PThread::makeWithRet([&]() {
    assert(thread5 == PThread::self());
    std::cout << "Hello thread 5" << std::endl;
    return &thread5;
  });

  auto ptrToTest = thread4.join<int>();
  thread2.join();
  thread3.join();

  auto thread5RetVal = thread5.join<PThread>();
  assert(thread5 == *thread5RetVal);
  assert(thread1 != thread2);

  std::cout << "Thread 4 returned " << *ptrToTest << std::endl;
  return 0;
}
