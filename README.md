# Project 3: In-Place Computing Sum of Large Arrays on Multicore Systems

### Note:
1. Compile **test-shm.c** as it contains the primary code.
2. Run the compiled file followed by two arguments: (1) a .txt file of white-spaced numbers, (2) the number of elements in the .txt file
3. A test file, **testfile-128.txt** has been added. This test file contains 128 white spaced numbers.

### Example Usage:
1. gcc -o test-shm test-shm.c
2. ./test-shm.c testfile-128.txt 128


## Project Description

### Objective
1. Divide an array by the number of cores (if using a multicore system), or by some M number of threads (if using multithreading).

2. Delegate the partial sum in each section to each individual thread/process.

3. Sum the partial sums and produce the output.

4. Use a single thread to sum the array and to compare the results.

### Optional Task

Divide the array using strides, where the stride is the number of cores or threads. Continue from step 2.