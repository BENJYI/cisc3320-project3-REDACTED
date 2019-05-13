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

int main(int argc, char *argv[]) {
  char *filename = argv[1];
  int array_size;
  sscanf(argv[2], "%i", &array_size);
  double *f;
  if ((f = malloc(array_size*sizeof(double))) == NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }
  fileToArray(filename, f, array_size);

  for (int i = 0; i < array_size; i++) {
    printf("%f ", f[i]);
  }

  return 0;
}