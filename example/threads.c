#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <string.h>

void *work_something(void *args) {
  sleep(5);

  printf("Test\n");

  kill(getpid(), SIGTERM);
  return NULL;
}

int main() {
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGINT);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    perror("sigprocmask");
    return 1;
  }

  pthread_t worker;
  pthread_attr_t worker_attr;

  if (pthread_attr_init(&worker_attr) != 0) {
    perror("pthread_attr_init");
    return 1;
  }

  if (pthread_attr_setdetachstate(&worker_attr, PTHREAD_CREATE_DETACHED) != 0) {
    perror("pthread_attr_setdetachstate");
    return 1;
  }

  if (pthread_create(&worker, &worker_attr, work_something, NULL) != 0) {
    perror("pthread_create");
    return 1;
  }

  int epoll_fd = epoll_create(1);
  if (epoll_fd == -1) {
    perror("epoll_create");
    return 1;
  }

  int signals = signalfd(-1, &mask, SFD_NONBLOCK);
  if (signals == -1) {
    perror("signalfd");
    return 1;
  }

  struct epoll_event events[10];
  struct epoll_event event;

  event.events = EPOLLIN;
  event.data.fd = signals;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, signals, &event) == -1) {
    perror("add signalfd to epoll");
    return 1;
  }

  while (1) {
    int ready = epoll_wait(epoll_fd, events, 100, -1);
    if (ready == -1) {
      perror("epoll_wait");
      return 1;
    }

    int i;
    for (i = 0; i < ready; i++) {
      if (events[i].data.fd == signals) {
        printf("Received signal.\n");

        struct signalfd_siginfo info;
        if (read(signals, &info, sizeof(info)) == -1) {
          perror("read");
          return 1;
        }

        printf("Signal number: %d\n", info.ssi_signo);
      }
    }
  }

  return 0;
}
