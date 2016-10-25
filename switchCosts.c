#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

void functionCall(){}

int main()
{
  struct timespec start;
  struct timespec stop;
  unsigned long long result; //64 bit integer

  clock_gettime(CLOCK_REALTIME, &start);
  sleep(1);
  clock_gettime(CLOCK_REALTIME, &stop);

  result=timespecDiff(&stop,&start);

  printf("CLOCK_REALTIME Measured: %llu\n",result);

  clock_gettime(CLOCK_MONOTONIC, &start);
  sleep(1);
  clock_gettime(CLOCK_MONOTONIC, &stop);

  result=timespecDiff(&stop,&start);

  printf("CLOCK_MONOTONIC Measured: %llu\n",result);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  sleep(1);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

  result=timespecDiff(&stop,&start);

  printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  sleep(1);
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

  result=timespecDiff(&stop,&start);

  printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);

  unsigned long long sum = 0;
  unsigned long long overhead;
  unsigned long long avg;
  int i;
  int N_iterations = 10000000;
  for (i = 0; i < N_iterations; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result = timespecDiff(&stop, &start);
    sum += result;
  }
  overhead = sum*1.0/N_iterations;
  printf("Overhead for CLOCK_PROCESS_CPUTIME_ID: %lluns\n", overhead);

  for (i = 0; i < N_iterations; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    functionCall();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result = timespecDiff(&stop, &start);
    sum += result;
  }
  avg = sum*1.0/N_iterations;
  printf("functionCall time is: %lluns\n", avg);
  printf("Total time with overhead removed: %lluns\n", avg-overhead);

  for (i = 0; i < N_iterations; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    getpid();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result = timespecDiff(&stop, &start);
    sum += result;
  }
  avg = sum*1.0/N_iterations;
  printf("getpid time is: %lluns\n", avg);
  printf("Total time with overhead removed: %lluns\n", avg-overhead);
}
