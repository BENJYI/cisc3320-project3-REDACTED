# Project 3: In-Place Computing Sum of Large Arrays on Multicore Systems

### Note:

1. Makefile is provided to compile all files, including utilities.
2. All experiments are prefixed with "test-".
3. All test files are prefixed with "testfile-N", where N is the number of elements, i.e. **testfile-4096** contains 4096 numbers. (These numbers should be whitespaced!)

### Usage:
1. `make` compiles all files.
2. `make clean` removes all compiled files.
3. Given in the repository are four test files, prefixed with "testfile-" followed by the number of elements.
4. A program requires two inputs, a text file with whitespaced numbers (integers or floating point), followed by the number of elements.

### Example:
1. `make`
2. `./test-shm.c testfile-262144.txt 262144`

---

## Project Description

### Objective
1. Divide an array by the number of cores (if using a multicore system), or by some M number of threads (if using multithreading).

2. Delegate the partial sum in each section to each individual thread/process.

3. Sum the partial sums and produce the output.

4. Use a single thread to sum the array and to compare the results.

### Optional Task

Divide the array using strides, where the stride is the number of cores or threads. Continue from step 2.