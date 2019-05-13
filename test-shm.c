#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>

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

void setPartialSum(double *f, int l, int r) {
  while (l+1 < r) {
    f[l+1] += f[l];
    l++;
  }
}

int main(int argc, char *argv[]) {
  char *filename = argv[1];
  int array_size, m; 
  m = 4; // number of theads/processes
  double *f;

  /* parse arguments */
  sscanf(argv[2], "%i", &array_size);
  if ((f = malloc(array_size*sizeof(double))) == NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }
  f = mmap(NULL, array_size*sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (f < 0) {
    perror("mmap()");
    exit(EXIT_FAILURE);
  }
  fileToArray(filename, f, array_size);
  int stride = array_size / m;

  double compareTotal = 0;
  for (int i = 0; i < array_size; i++) {
    compareTotal += f[i];
  }

  /* fork processes and get partial sums */
  for (int id = 0; id < m; id++) {
    if (fork() == 0) {
      setPartialSum(f, id*stride, (id+1)*stride);
      exit(0);  
    } else {
      wait(NULL);
    }
  }

  /* read partial sums from each child */
  for (int i = stride*2-1; i <= array_size; i+=stride) {
    f[i] += f[i-stride];
  }
  
  double total = f[array_size-1];
  /* check sum */
  if (total == compareTotal) {
    printf("Successfully summed numbers to: %f\n", total);
  } else {
    printf("Something went wrong.\n");
    printf("Total should be: %f, but is %f\n", compareTotal, total);
  }
  return 0;
}