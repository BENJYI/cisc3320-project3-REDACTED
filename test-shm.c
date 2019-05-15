#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <semaphore.h>

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

void setPartialSum(double *f, int l, int r, int array_size, int *lock) {
  if (r == array_size) r-=1;
  while (l+1 < r) {
    f[l+1] += f[l];
    l++;
  }
  
  /* since a race condition is not possible when each process
     is access its own section of the array, we just take the
     unnecessary step of always summing at one specific index. */
  
  // wait()

  sem_wait(&mutex);
  f[array_size-1] += f[l];
  sem_post(&mutex);
}

int main(int argc, char *argv[]) {
  char *filename = argv[1];
  int array_size, m; 
  size_t size;
  m = get_nprocs(); // number of theads/processes
  
  double *f;
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
  f = mmap(NULL, array_size*sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  lock = 1;
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
      /* set processor to child */
      if (sched_setaffinity(getpid(), sizeof(set), set) < 0) {
        perror("sched_setaffinity()");
        exit(EXIT_FAILURE);
      }
      setPartialSum(f, id*stride, (id+1)*stride, array_size);
      exit(0);  
    } else {
      wait(NULL);
    }
  }

  // /* read partial sums from each child */
  // for (int i = stride*2-1; i <= array_size; i+=stride) {
  //   f[i] += f[i-stride];
  // }
  
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
