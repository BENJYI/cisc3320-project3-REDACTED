#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

double getPartialSum(int i, double *f, int l, int r) {
  double sum = 0;
  for(int i = l; i < r; i++) {
    sum += f[i];
  }
  printf("PART %d IS DONE!!! SUM IS: %f\n", i, sum);
  return sum;
}

int main(int argc, char *argv[]) {
  char *filename = argv[1];
  int array_size, m;
  int status = 0;
  double *f;

  /* argument parsing */
  sscanf(argv[2], "%i", &array_size);
  if ((f = malloc(array_size*sizeof(double))) == NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }
  fileToArray(filename, f, array_size);

  // Check for number of threads
  m = 4;
  int range = array_size / m;

  double total = 0;
  // Split array by stride
  for (int i = 0; i < m; i++) {
    double sum;
    if (fork() == 0) {
      printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
      // sum = getPartialSum(i, f, i*range, (i+1)*range);  
    }
    wait(NULL);
    total += sum;
  }
  
  // double compareTotal = 0;
  // for (int i = 0; i < array_size; i++) {
  //   compareTotal += f[i];
  // }
  // printf("%f == %f ?", total, compareTotal);
  return 0;
}