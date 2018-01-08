#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>

double timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


void functionCall(){}

int processSwitch();

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condT1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condT2 = PTHREAD_COND_INITIALIZER;
int *flag;
void *thread(void *f);
void threadSwitch();

struct timespec start;
struct timespec stop;

int main()
{
  double result; //64 bit integer 
  double sum = 0;
  double overhead;
  double avg;
  int i;
  int N_iterations = 10000000;

  /*
    Calculates amount of time clock_gettime(CLOCK_PROCESS_CPUTIME_ID) takes
    to call.
  */
  sum = 0;
  for (i = 0; i < N_iterations; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result = timespecDiff(&stop, &start);
    sum += result;
  }
  overhead = sum*1.0/N_iterations;
  printf("Overhead for CLOCK_PROCESS_CPUTIME_ID: %fns\n", overhead);

  /*
    Calculates amount of time a minimal function call takes.
  */
  sum = 0;
  for (i = 0; i < N_iterations; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    functionCall();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result = timespecDiff(&stop, &start);

    sum += result;
  }
  avg = sum*1.0/N_iterations;
  printf("functionCall time is: %fns\n", avg);
  printf("functionCall time with overhead removed: %fns\n", avg-overhead); //subtracts clock_gettime 

  /*
    Calculates amount of time a minimal system call takes
  */
  // FILE *f = fopen("getpid.csv", "w+");
  sum = 0;
  for (i = 0; i < N_iterations; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    getpid();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result = timespecDiff(&stop, &start);
    // fprintf(f,"%f,", result);
    sum += result;
  }
  // fclose(f);
  avg = sum*1.0/N_iterations;
  printf("getpid time is: %fns\n", avg);
  printf("getpid time with overhead removed: %fns\n", avg-overhead); //subtracts clock_gettime()

  // /*
  //   Calculates amount of time a single read and write takes
  // */
  // char readbuffer[] = "A";
  // char inputbuffer[] = "B";
  // int p[2];
  // pipe(p);
  // sum = 0;
  // for (i = 0; i< N_iterations; i++) {
  //   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  //   write(p[1], inputbuffer, sizeof(inputbuffer));
  //   read(p[0], readbuffer, sizeof(readbuffer));
  //   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

  //   result = timespecDiff(&stop, &start);
  //   sum += result;
  // }
  // avg = sum*1.0/N_iterations;
  // printf("Single read+write avg: %f\n", avg);
  // double context = overhead + avg*2.0;
  // printf("Total 2 read+write + gettime: %f\n", context);

  // int smallerN = 100000;
  // /*
  //   Calculates the amount of time calling process switch takes
  //   Since in each call there are 2 context switches final result is halved
  // */
  // sum = 0;
  // for (i = 0; i < smallerN; i++) {
  //   processSwitch();
  //   result = timespecDiff(&stop, &start);
  //   sum += result;
  // }
  // avg = sum*1.0/smallerN;
  // printf("ProcessSwtich time for all: %f\n", avg);
  // printf("ProcessSwitch subtract extra reads+writes: %f\n", avg - context);
  // printf("Single context Switch: %f\n", (avg-context)/2.0);
  

  /*
    Calculates amount of time a thread takes to swap a flag back and forth
  */
  int small = 1000;
  sum = 0;
  for(i = 0; i < small; i++) {
    threadSwitch();
    result = timespecDiff(&stop, &start);
    sum += result;
  }
  avg = sum*1.0/small;
  result = timespecDiff(&stop, &start);
  printf("threadSwitch time for all: %f\n", avg);

  // /*
  //   Calculates how long it takes to lock and unlock a thread and the time
  //   it spends in the critical section
  // */
  // sum = 0;
  // for(i = 0; i < smallerN; i++) {
  //   clock_gettime(CLOCK_MONOTONIC, &start);
  //   pthread_mutex_lock(&lock);
  //   pthread_mutex_unlock(&lock);
  //   clock_gettime(CLOCK_MONOTONIC, &stop);

  //   result = timespecDiff(&stop, &start);
  //   sum +=result;
  // }
  // avg = sum*1.0/smallerN;
  // printf("Lock+Unlock: %f\n", avg);
  // double con = overhead + avg*2.0;
  // printf("threadSwitch subtract locks: %f\n", avg-con);
  // printf("single thread Switch: %f\n", (avg-con)/2.0);

  return 0;
}

/*
  Simple process that passes a single char message between them

  modified from pipes.c sample code from course website
*/
int processSwitch(){
  cpu_set_t mask; // tells what core to use
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);

  int pipeP[2]; // pipe child uses to contact parent
  int pipeC[2]; // pipe parent uses to contact child 

  pid_t childpid;
  int singleByte;
  char readbuffer[] = "A"; 
  char inputbuffer[] = "B"; // parent sends to child
  char returnstring[] = "C"; // child sends to parent

  pipe(pipeP); // pipe for parent to communicate to child
  pipe(pipeC); // pipe for child to communicate to parent

  childpid = fork();
  sched_setaffinity(childpid, sizeof(mask), &mask); // makes sure both processes are on the same core
  if (childpid == -1) {
    perror("fork");
    exit(1);
  } else if (childpid == 0) {
    close(pipeC[0]); // closes output side of child pipe
    close(pipeP[1]); // closes input side of parent pipe

    singleByte = read(pipeP[0], readbuffer, sizeof(readbuffer)); // read message from parent
    // printf("Parent Sends: %s\n", readbuffer); 
    write(pipeC[1], returnstring, (strlen(returnstring))); // send ACK
    exit(0);
  } else {
    close(pipeC[1]); // closes input side of child pipe
    close(pipeP[0]); // closes output side of parent pipe

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); //start timer
    write(pipeP[1], inputbuffer, (strlen(inputbuffer))); // sends message to child
    singleByte = read(pipeC[0], readbuffer, sizeof(readbuffer)); // reads ACK from child
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop); //stop timer
    // printf("Child send: %s\n", readbuffer);

    wait(NULL);
    return(0);
  }
}

/*
  Process That creates two threads and passes a flag between to calculate the thread
  context cost.
*/
void *thread1(void *f);
void *thread2(void *f);
void threadSwitch() {
  flag = (int *) malloc(sizeof(int)); //allocates flag to size int
  *flag = 1;
  pthread_t t1, t2; //creates two threads
  cpu_set_t mask; //sets up for making sure the thread runs on the same processor
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);

  sched_setaffinity(t1, sizeof(mask), &mask); //makes t1 to be on one core
  pthread_create(&t1, NULL, thread1, flag); // creates thread 1
  sched_setaffinity(t2, sizeof(mask), &mask); // makes t2 to be same core as t1
  pthread_create(&t2, NULL, thread2, flag); // creates thread 2

  pthread_join(t1, NULL); // joins the thread/destroys thread 1
  pthread_join(t2, NULL); // joins the thread/destroys thread 2
}

/*
  Process for thread1 to swap value of flag
*/
void *thread1(void *f) {
  int init = *flag;
  while(init == *flag) {
    pthread_mutex_lock(&lock);
    while(*flag != 1)
      pthread_cond_wait(&condT1, &lock); // conditional wait for thread2 to change value
    pthread_mutex_unlock(&lock);

    clock_gettime(CLOCK_MONOTONIC, &start); //start timer

    pthread_mutex_lock(&lock);
    *flag = 0;
    pthread_cond_signal(&condT2);
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(0);
}


/*
  Process for thread 2 to swap value of flag
*/
void *thread2(void *f) {
  int init = *flag;
  while(init == *flag) {
    pthread_mutex_lock(&lock);
    while(*flag != 0)
      pthread_cond_wait(&condT2, &lock);
    pthread_mutex_unlock(&lock);

    clock_gettime(CLOCK_MONOTONIC, &stop); // stop

    pthread_mutex_lock(&lock);
    *flag = 1;
    pthread_cond_signal(&condT1);
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(0);
}