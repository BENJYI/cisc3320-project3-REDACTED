# Project 3: In-Place Computing Sum of Large Arrays on Multicore Systems

### Objective
1. Divide an array by the number of cores (if using a multicore system), or by some M number of threads (if using multithreading).

2. Delegate the partial sum in each section to each individual thread/process.

3. Sum the partial sums and produce the output.

4. Use a single thread to sum the array and to compare the results.

### Optional Task

Divide the array using strides, where the stride is the number of cores or threads. Continue from step 2.