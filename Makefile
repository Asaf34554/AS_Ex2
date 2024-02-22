.PHONY: all clean

all: randomGenerator primesCounter primesCounter2

randomGenerator: generator.c
	gcc -o randomGenerator generator.c

primesCounter: primeCountersThread.c tpool.c
	gcc -o primesCounter primeCountersThread.c tpool.c -lpthread

primesCounter2: primeCounter.c 
	gcc -o primesCounter2 primeCounter.c
clean:
	-rm randomGenerator primeCounter primesCounter2 2>/dev/null
