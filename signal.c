#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <errno.h>

static int signalled = 0;

static _Atomic int counter;

void sighandler(int signum) {
  signalled = 1;
}

void delay(double d) {
  struct timespec t;
  int ret;
  int loop = 0;
  t.tv_sec = (time_t) d;
  t.tv_nsec = (d - t.tv_sec) * 1e9;
  do {
    if (loop) {
      putchar('x'); fflush(stdout);
    }
    ret = nanosleep(&t, &t);
    loop++;
  } while (ret == -1 && errno == EINTR && t.tv_sec < 5);
  if (ret == -1) {
    fprintf(stderr, " - nanosleep failed: %d, t = %lld, %lld\n", errno, t.tv_sec, t.tv_nsec);
    exit(2);
  }
}

void print_message(double d, char c) {
  while (!signalled && atomic_load(&counter) <= 20) {
    atomic_fetch_add(&counter, 1);
    putchar(c);
    fflush(stdout);
    delay(d);
  }
}

void * thread(void *arg) {
  print_message(0.6666666666, 'a');
}

int main (void) {
  struct sigaction sigact, oldsigact;
  pthread_t thr;
  pthread_attr_t attr;
  pthread_cond_t done;

  sigact.sa_handler = &sighandler;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = 0;
  if (sigaction(SIGINT, &sigact, &oldsigact) == -1) {
    fprintf(stderr, "Signal handler failed\n");
    return 2;
  }
  atomic_init(&counter, 0);

  if (pthread_cond_init(&done, NULL) != 0) {
    fprintf(stderr, "Cond creation failed\n");
    return 2;
  }

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&thr, &attr, &thread, NULL) != 0) {
    fprintf(stderr, "Thread creation failed\n");
    return 2;
  }

  print_message(1.0, 'b');

  pthread_join(thr, NULL);

  if (signalled) {
    printf(" - Got CTRL+C, exiting\n");
    return 0;
  } else {
    printf(" - not signalled???\n");
    return 2;
  }
}
