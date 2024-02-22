#ifndef PRIME_UTILS_H
#define PRIME_UTILS_H

#include <stdbool.h>

// Function to check if a number is prime
int isPrime(int n);

// Worker function to process a single number
void worker(void* arg);

#endif /* PRIME_UTILS_H */
