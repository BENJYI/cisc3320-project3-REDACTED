#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

void fileToArray(char *filename, double *f, int array_size) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("fileToArray()");
    exit(EXIT_FAILURE);
  }
  double x;
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
  m = 4; // number of theads/processes
  
  int status = 0;
  int range = array_size / m;
  int fd[2*m];
  double *f;

  /* parse arguments */
  sscanf(argv[2], "%i", &array_size);
  if ((f = malloc(array_size*sizeof(double))) == NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }
  fileToArray(filename, f, array_size);

  /* create pipes for each child */
  for (int i = 0; i < m; i++) {
    if (pipe(&fd[2*i]) < 0) {
      perror("pipe()");
    }
  }

  /* fork processes and get partial sums */
  for (int id = 0; id < m; id++) {
    if (fork() == 0) {
      getPartialSum(fd, id, f, id*range, (id+1)*range);
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
  
  /* single-thread summing */
  double compareTotal = 0;
  for (int i = 0; i < array_size; i++) {
    compareTotal += f[i];
  }

  /* check sum */
  if (total == compareTotal) {
    printf("Successfully summed numbers to: %f\n", total);
  } else {
    printf("Something went wrong.\n");
  }
  return 0;
}