#ifndef __TPOOL_H__
#define __TPOOL_H__

#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>

// Forward declaration of the tpool struct
struct tpool;

// Typedef for the tpool struct
typedef struct tpool tpool_t;

// Typedef for the thread function pointer
typedef void (*thread_func_t)(void *arg);

// Function prototypes

/**
 * @brief Creates a new thread pool.
 * 
 * @param num Number of threads in the pool.
 * @return A pointer to the newly created thread pool.
 */
tpool_t *tpool_create(size_t num);

/**
 * @brief Destroys a thread pool.
 * 
 * @param tm Pointer to the thread pool to be destroyed.
 */
void tpool_destroy(tpool_t *tm);

/**
 * @brief Adds a work item to the thread pool.
 * 
 * @param tm Pointer to the thread pool.
 * @param func Pointer to the function to be executed by the thread.
 * @param arg Argument to be passed to the function.
 * @return true if the work item was successfully added, false otherwise.
 */
bool tpool_add_work(tpool_t *tm, thread_func_t func, void *arg);

/**
 * @brief Waits for all work items in the thread pool to complete.
 * 
 * @param tm Pointer to the thread pool.
 */
void tpool_wait(tpool_t *tm);

#endif /* __TPOOL_H__ */
