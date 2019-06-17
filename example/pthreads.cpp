#include <pthread.h>
#include <unistd.h>
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
  pthread_t thread1, thread2;
  if (int err = pthread_create(&thread1, nullptr, &thread1Routine, nullptr)) {
    std::cerr << "pthread_create error code: " << err << std::endl;
    return 1;
  }

  if (int err = pthread_create(&thread2, nullptr, &thread2Routine, nullptr)) {
    std::cerr << "pthread_create error code: " << err << std::endl;
    return 1;
  }

  if (int err = pthread_join(thread1, nullptr)) {
    std::cerr << "pthread_join error code: " << err << std::endl;
    return 2;
  }

  return 0;
}
