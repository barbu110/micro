// Copyright 2019 Stefan Silviu-Alexandru

#include <cassert>
#include <iostream>
#include <pthread_wrapper.h>
#include <unistd.h>

using namespace microloop;

void* thread1_routine(void*)
{
  sleep(2);
  std::cout << "Hello thread 1" << std::endl;
  return nullptr;
}

void* thread2_routine(void*)
{
  sleep(1);
  std::cout << "Hello thread 2" << std::endl;
  return nullptr;
}

int main()
{
  Thread thread1(thread1_routine);
  thread1.detach();

  Thread thread2(thread2_routine,
      ThreadAttributes()
          .set_detach_state(JOINABLE)
          .set_scope(SYSTEM)
          .set_inherit_scheduler(INHERIT)
          .set_scheduling_policy(OTHER)
          .set_scheduling_priority(0)
          .set_guard_size(4096)
          .set_guard_size(0x201000));

  int thread3_return;

  Thread thread3([&thread3_return]() {
    sleep(1);
    std::cout << "Hello thread 3" << std::endl;
    thread3_return = 123;
  });
  thread3.detach();

  int test = 42;

  Thread thread4 = Thread::make_with_ret([&]() {
    std::cout << "Hello thread 4" << std::endl;
    return &test;
  });

  Thread thread5 = Thread::make_with_ret([&]() {
    assert(thread5 == Thread::self());
    std::cout << "Hello thread 5" << std::endl;
    return &thread5;
  });

  auto ptr_to_test = thread4.join<int>();
  thread2.join();

  auto thread5_ret_val = thread5.join<Thread>();
  assert(thread5 == *thread5_ret_val);
  assert(thread1 != thread2);

  std::cout << "Thread 3 returned " << thread3_return << std::endl;
  std::cout << "Thread 4 returned " << *ptr_to_test << std::endl;
  return 0;
}
