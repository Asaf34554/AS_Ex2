# Prime Number Counter with Threading

## Overview
This project implements a prime number counter that utilizes threading to process an endless stream of numbers efficiently. The goal is to parallelize the prime number checking process to fully utilize CPU cores and improve performance.

## Features
- Utilizes a thread pool for parallel processing.
- Implements a simple primality test function for checking prime numbers.
- Processes numbers in batches to optimize parallelism.
- Monitors CPU and memory usage to ensure efficiency and compliance with requirements.
- Provides benchmarking results for performance evaluation.

## Usage
1. **Compilation**: Compile the source files using a C compiler. For example:
    ```
    gcc -o primeCounter primeCountersThread.c tpool.c prime_utils.c -lpthread
    ```
2. **Execution**: Run the compiled program with input from the provided random number generator. For example:
    ```
    ./randomGenerator 10 100 | ./primeCounter
    ```
    This command generates 100 numbers using a seed of 10 and pipes them as input to the prime number counter.

## Requirements
- C compiler (e.g., GCC)
- POSIX-compliant operating system (for threading support)
- Standard C libraries

## Files
- `primeCountersThread.c`: Main source file containing the prime number counter implementation.
- `tpool.c` and `tpool.h`: Source and header files for the thread pool implementation.
- `prime_utils.c` and `prime_utils.h`: Source and header files containing utility functions for prime number operations.
- `randomGenerator`: Executable for generating random numbers (provided).

## Performance
- The performance of the prime number counter can be evaluated using the `time` command for timing and profiling tools like `valgrind` for memory usage analysis.
- Benchmarking results should be compared against provided reference results to ensure compliance with performance targets.

## Future Improvements
- Explore more optimized algorithms for prime number checking.
- Fine-tune thread pool parameters for better performance.
- Implement dynamic adjustment of thread pool size based on system specifications.
- Enhance error handling and input validation for robustness.

## Authors
- Assaf Shmaryahu
- Lior jerbi