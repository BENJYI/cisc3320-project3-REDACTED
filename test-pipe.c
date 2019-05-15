#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <semaphore.h>
#include "util.h"

sem_t mutex;

void fileToArray(char *filename, double *f, int array_size) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) { 
    perror("fileToArray()");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < array_size; i++) {
    fscanf(file, "%lf", &f[i]);
  }
}

void getPartialSum(int *fd, int id, double *f, int l, int r) {
  close(fd[2*id]);
  double sum = 0;
  for(int i = l; i < r; i++) {
    sum += f[i];
  }
  write(fd[2*id+1], &sum, sizeof(double));
  close(fd[2*id+1]);
}

int main(int argc, char *argv[]) {
    char *filename = argv[1];
  int array_size, m; 
  size_t size;
  m = get_nprocs(); // number of theads/processes
  double start, time1, time2;
  
  double *f;
  int fd[2*m];
  sem_init(&mutex, 0, 1);
  struct sched_param sp;
  cpu_set_t *set;
  set = CPU_ALLOC(m);
  if (set == NULL) {
    perror("CPU_ALLOC()");
   exit(EXIT_FAILURE);
  }
  
  /* initialize cpu sets */
  size = CPU_ALLOC_SIZE(m);
  CPU_ZERO_S(size, set);
  for (int i = 0; i < m; i++) {
    CPU_SET_S(i, size, set);
  }

  /* configure scheduler */
  sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
  if (sched_setscheduler(0, SCHED_FIFO, &sp) < 0) {
    perror("sched_setscheduler()");
    exit(EXIT_FAILURE);
  }

  /* parse arguments */
  sscanf(argv[2], "%i", &array_size);
  if ((f = malloc(array_size*sizeof(double))) == NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }
  if (f < 0) {
    perror("mmap()");
    exit(EXIT_FAILURE);
  }
  fileToArray(filename, f, array_size);
  int stride = array_size / m;

  /* create pipes for each child */
  for (int i = 0; i < m; i++) {
    if (pipe(&fd[2*i]) < 0) {
      perror("pipe()");
    }
  }

  double compareTotal = 0;
  start = gethrtime_x86();
  for (int i = 0; i < array_size; i++) {
    compareTotal += f[i];
  }
  time1 = gethrtime_x86()-start;

  start = gethrtime_x86();
  /* fork processes and get partial sums */
  for (int id = 0; id < m; id++) {
    if (fork() == 0) {
      /* set processor to child */
      if (sched_setaffinity(getpid(), sizeof(set), set) < 0) {
        perror("sched_setaffinity()");
        exit(EXIT_FAILURE);
      }
      getPartialSum(fd, id, f, id*stride, (id+1)*stride);
      exit(0);  
    } else {
      wait(NULL);
    }
  }

  /* read partial sums from each child */
  double total = 0;
  for (int i = 0; i < m; i++) {
    double sum; 
    read(fd[2*i], &sum, sizeof(double));
    close(fd[2*i+1]);
    total += sum;
  }

  time2 = gethrtime_x86()-start;

  printf("\nResults: \n");
  printf("No. of Cores: %d\n", m);
  printf("Target sum: %f\n", compareTotal);
  printf("=======================================\n");
  if (total == compareTotal) {
    printf("Successfully summed numbers to: %f\n", total);
    printf("Single thread time: %.06fs\n", time1);
    printf("Multi-thread time:  %.06fs\n", time2);
  } else {
    printf("Something went wrong.\n");
    printf("Total should be: %f, but is %f\n", compareTotal, total);
  }
  printf("\n");
  return 0;
}