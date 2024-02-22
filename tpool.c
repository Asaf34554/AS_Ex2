#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "tpool.h"

// Struct definition for a work item in the thread pool
struct tpool_work {
    thread_func_t func;         // Pointer to the function to be executed
    void *arg;                  // Argument to be passed to the function
    struct tpool_work *next;    // Pointer to the next work item
};

typedef struct tpool_work tpool_work_t;

// Struct definition for the thread pool
struct tpool {
    tpool_work_t *work_first;   // Pointer to the first work item in the queue
    tpool_work_t *work_last;    // Pointer to the last work item in the queue
    pthread_mutex_t work_mutex; // Mutex for thread safety when accessing the queue
    pthread_cond_t work_cond;   // Condition variable for signaling when work is available
    size_t thread_cnt;          // Number of threads in the pool
    size_t active_cnt;          // Number of active threads
    bool stop;                  // Flag indicating whether the pool should stop
};

// Function to create a new work item
static tpool_work_t *tpool_work_create(thread_func_t func, void *arg)
{
    tpool_work_t *work = malloc(sizeof(*work));  // Allocate memory for the work item
    if (work == NULL) {
        perror("Failed to allocate memory for work item");
        return NULL;
    }
    work->func = func;  // Set the function pointer
    work->arg = arg;    // Set the argument
    work->next = NULL;  // Initialize next pointer to NULL
    return work;
}

// Function to destroy a work item
static void tpool_work_destroy(tpool_work_t *work)
{
    if (work != NULL) {
        free(work);  // Free memory allocated for the work item
    }
}

// Function to retrieve a work item from the queue
static tpool_work_t *tpool_work_get(tpool_t *tm)
{
    if (tm == NULL || tm->work_first == NULL)
        return NULL;

    tpool_work_t *work = tm->work_first;  // Get the first work item
    tm->work_first = work->next;          // Move the pointer to the next item

    if (tm->work_first == NULL) {
        tm->work_last = NULL;  // If no more items in the queue, set last to NULL
    }

    return work;
}

// Worker function that runs in each thread
static void *tpool_worker(void *arg)
{
    tpool_t *tm = arg;
    while (true) {
        pthread_mutex_lock(&(tm->work_mutex));  // Lock the mutex before accessing the queue
        while (tm->work_first == NULL && !tm->stop) {
            pthread_cond_wait(&(tm->work_cond), &(tm->work_mutex));  // Wait for work to be available
        }
        if (tm->stop) {
            pthread_mutex_unlock(&(tm->work_mutex));  // Unlock mutex and exit if pool is stopping
            break;
        }
        tpool_work_t *work = tpool_work_get(tm);  // Get a work item from the queue
        if (work != NULL) {
            tm->active_cnt++;  // Increment active count if work item obtained
        }
        pthread_mutex_unlock(&(tm->work_mutex));  // Unlock mutex before executing the work item

        if (work != NULL) {
            work->func(work->arg);             // Execute the function associated with the work item
            tpool_work_destroy(work);          // Destroy the work item after execution
        }

        pthread_mutex_lock(&(tm->work_mutex));  // Lock the mutex before modifying active count
        tm->active_cnt--;                       // Decrement active count after work item execution
        if (!tm->stop && tm->work_first == NULL && tm->active_cnt == 0) {
            pthread_cond_signal(&(tm->work_cond));  // Signal if no more work and no active threads
        }
        pthread_mutex_unlock(&(tm->work_mutex));    // Unlock mutex after modifying active count
    }

    tm->thread_cnt--;                    // Decrement thread count when thread exits
    if (tm->thread_cnt == 0) {
        pthread_cond_signal(&(tm->work_cond));  // Signal if no more threads
    }
    pthread_mutex_unlock(&(tm->work_mutex));   // Unlock mutex before exiting
    return NULL;
}

// Function to create a new thread pool
tpool_t *tpool_create(size_t num)
{
    if (num == 0) {
        num = 2;  // Default to 2 threads if number is 0
    }

    tpool_t *tm = calloc(1, sizeof(*tm));  // Allocate memory for the thread pool
    if (tm == NULL) {
        perror("Failed to allocate memory for thread pool");
        return NULL;
    }

    tm->thread_cnt = num;                           // Set the number of threads
    pthread_mutex_init(&(tm->work_mutex), NULL);    // Initialize the mutex
    pthread_cond_init(&(tm->work_cond), NULL);      // Initialize the condition variable
    tm->work_first = NULL;                          // Initialize the work queue
    tm->work_last = NULL;
    for (size_t i = 0; i < num; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, tpool_worker, tm);  // Create threads for the pool
        pthread_detach(thread);  // Detach threads to allow them to clean up after completion
    }
    return tm;
}

// Function to destroy a thread pool
void tpool_destroy(tpool_t *tm)
{
    if (tm != NULL) {
        pthread_mutex_lock(&(tm->work_mutex));  // Lock the mutex before destroying
        tm->stop = true;                        // Set stop flag to true
        pthread_cond_broadcast(&(tm->work_cond));  // Signal all threads to stop
        while (tm->thread_cnt > 0) {
            pthread_cond_wait(&(tm->work_cond), &(tm->work_mutex));  // Wait for all threads to exit
        }
        pthread_mutex_unlock(&(tm->work_mutex));  // Unlock mutex after all threads have exited
        pthread_mutex_destroy(&(tm->work_mutex));  // Destroy the mutex
        pthread_cond_destroy(&(tm->work_cond));    // Destroy the condition variable
        free(tm);                                  // Free memory allocated for the thread pool
    }
}

// Function to add a work item to the thread pool
bool tpool_add_work(tpool_t *tm, thread_func_t func, void *arg)
{
    if (tm == NULL || func == NULL) {
        return false;  // Return false if thread pool or function pointer is NULL
    }

    tpool_work_t *work = tpool_work_create(func, arg);  // Create a new work item
    if (work == NULL) {
        return false;  // Return false if failed to create work item
    }

    pthread_mutex_lock(&(tm->work_mutex));  // Lock the mutex before accessing the queue
    if (tm->work_first == NULL) {
        tm->work_first = work;          // If queue is empty, set work as first and last item
        tm->work_last = tm->work_first;
    } else {
        tm->work_last->next = work;     // Otherwise, append work to the end of the queue
        tm->work_last = work;
    }
    pthread_cond_signal(&(tm->work_cond));  // Signal that work is available
    pthread_mutex_unlock(&(tm->work_mutex));  // Unlock mutex after modifying the queue
    return true;  // Return true indicating successful addition of work item
}

// Function to wait for all work items in the thread pool to complete
void tpool_wait(tpool_t *tm)
{
    if (tm != NULL) {
        pthread_mutex_lock(&(tm->work_mutex));  // Lock the mutex before waiting
        while (tm->work_first != NULL || tm->active_cnt > 0) {
            pthread_cond_wait(&(tm->work_cond), &(tm->work_mutex));  // Wait until no more work and no active threads
        }
        pthread_mutex_unlock(&(tm->work_mutex));  // Unlock mutex after all work items are completed
    }
}
