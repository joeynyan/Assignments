#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

void processSwitch();
void functionCall(){}

int main()
{
  struct timespec start;
  struct timespec stop;
  unsigned long long result; //64 bit integer

  // clock_gettime(CLOCK_REALTIME, &start);
  // sleep(1);
  // clock_gettime(CLOCK_REALTIME, &stop);

  // result=timespecDiff(&stop,&start);

  // printf("CLOCK_REALTIME Measured: %llu\n",result);

  // clock_gettime(CLOCK_MONOTONIC, &start);
  // sleep(1);
  // clock_gettime(CLOCK_MONOTONIC, &stop);

  // result=timespecDiff(&stop,&start);

  // printf("CLOCK_MONOTONIC Measured: %llu\n",result);

  // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  // sleep(1);
  // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

  // result=timespecDiff(&stop,&start);

  // printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

  // clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  // sleep(1);
  // clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

  // result=timespecDiff(&stop,&start);

  // printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);

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

  processSwitch();
}


void processSwitch(){
  int pipeP[2]; // pipe child uses to contact parent
  int pipeC[2]; // pipe parent uses to contact child 

  pid_t childpid;
  int singleByte;
  char readbuffer[] = "A";
  char inputbuffer[] = "B";
  char returnstring[] = "C";

  pipe(pipeP);
  pipe(pipeC);

  childpid = fork();
  if (childpid == -1) {
    perror("fork");
    exit(1);
  } else if (childpid == 0) {
    close(pipeC[0]); // closes output side of child pipe
    close(pipeP[1]); // closes input side of parent pipe

    singleByte = read(pipeP[0], readbuffer, sizeof(readbuffer));
    printf("Parent Sends: %s\n", readbuffer);
    write(pipeC[1], returnstring, (strlen(returnstring)));
  } else {
    close(pipeC[1]); // closes input side of child pipe
    close(pipeP[0]); // closes output side of parent pipe

    write(pipeP[1], inputbuffer, (strlen(inputbuffer)));
    singleByte = read(pipeC[0], readbuffer, sizeof(readbuffer));
    printf("Child send: %s\n", readbuffer);

    wait(NULL);
    // return(0);
  }
}
