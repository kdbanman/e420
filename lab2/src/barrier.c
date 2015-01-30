/* 
 * Kirby Banman
 * ECE 420 Lab 1
 * 15 Jan 2015
 *
 * File:  
 *    barrier.c
 *
 * Purpose:
 *    Use semaphores to provide a thread barrier.
 *    
 * Compile:  gcc -Wall -Wextra -o barrier barrier.c -lpthread
 */

#include <stdlib.h>
#include <semaphore.h>

typedef struct barrier {
  int thread_count;
  int blocked_thread_count;
  sem_t count_protector;
  sem_t barrier_sem;
} barrier;

int barrier_init(barrier *bar, int thread_count)
{
  int err;
  
  bar->thread_count = thread_count;
  bar->blocked_thread_count = 0;
  if ((err = sem_init(&(bar->count_protector), 0, 1))) return err;
  if ((err = sem_init(&(bar->barrier_sem), 0, 0))) return err;

  return 0;
}

int barrier_destroy(barrier *bar)
{
  int err;

  if ((err = sem_destroy(&(bar->count_protector)))) return err;
  if ((err = sem_destroy(&(bar->barrier_sem)))) return err;

  return 0;
}

// returns nonzero for the final (barrier-releasing) thread
int barrier_wait(barrier *bar)
{
  /* Wait for all threads to finish iteration */
  sem_wait(&(bar->count_protector));
  if (bar->blocked_thread_count < bar->thread_count - 1) {
    /* this is not the last thread to hit the barrier.  wait. */
    bar->blocked_thread_count++;
    sem_post(&(bar->count_protector));
    sem_wait(&(bar->barrier_sem));

    return 0;
  } else {
    /* this is the last thread to hit the barrier, release the others. */
    bar->blocked_thread_count = 0;
    sem_post(&(bar->count_protector));

    int i;
    for (i = 0; i < bar->thread_count - 1; i++) {
      sem_post(&(bar->barrier_sem));
    }

    return 1;
  }
}
