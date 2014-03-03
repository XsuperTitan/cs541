/*
 * Josh Hursey
 * Oct. 10, 2013
 *
 * CS 441/541: Semaphore Wrapper Library
 *
 */
#include "semaphore_support.h"


/*
 * Broad permissions for the semaphore
 */
#define S_ALL (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)


int semaphore_create(semaphore_t *sem, unsigned int value)
{
    /* Initialize the variables */
    sem->sem  = NULL;
    sem->name = NULL;

#if USE_NAMED_SEMAPHORES == 0
    static int sem_count = 0;

    asprintf(&(sem->name), "/cs441-%d-%4d", getpid(), sem_count);
    if( NULL == sem->name ) {
        fprintf(stderr, "Error: semaphore_create(): Failed to allocate memory for the name\n");
        return -1;
    }
    sem_count++;

    /* printf("Creating a semaphore named: %s\n", sem->name); */

    sem->sem = sem_open(sem->name, O_CREAT|O_EXCL, S_ALL, value);
    if( SEM_FAILED == sem->sem ) {
        perror("Error: sem_open failed!");
        fprintf(stderr, "Failed on Semaphore named %s\n", sem->name);
        return -2;
    }

    return 0;
#else
    /* printf("Creating a semaphore named: UNNAMED (value %d)\n", value); */
    sem->sem = (sem_t*)malloc(sizeof(sem_t) * 1);
    if( NULL == sem->sem ) {
        fprintf(stderr, "Error: semaphore_create(): Failed to allocate memory for the semaphore\n");
        return -1;
    }

    return sem_init(sem->sem, 0, value);
#endif
}


int semaphore_destroy(semaphore_t *sem)
{
    int rtn;

#if USE_NAMED_SEMAPHORES == 0
    rtn = sem_close(sem->sem);
    if( 0 != rtn ) {
        fprintf(stderr, "Error: semaphore_destroy(): sem_close failed with %d (skip sem_unlink)\n", rtn);
    } else {
        rtn = sem_unlink(sem->name);
    }
#else
    rtn = sem_destroy(sem->sem);

    if( NULL != sem->sem ) {
        free(sem->sem);
        sem->sem = NULL;
    }
#endif

    if( NULL != sem->name ) {
        free(sem->name);
        sem->name = NULL;
    }
    sem->sem  = NULL;

    return rtn;
}


int semaphore_wait(semaphore_t *sem)
{
    /*
     * Sanity checks
     */
    if( NULL == sem ) {
        fprintf(stderr, "Error: semaphore_wait(): Invalid argument!\n");
        return -2;
    }

    if( NULL == sem->sem ) {
        fprintf(stderr, "Error: semaphore_wait(): Semaphore has not been created yet!\n");
        return -3;
    }

    return sem_wait(sem->sem);
}


int semaphore_post(semaphore_t *sem)
{
    /*
     * Sanity checks
     */
    if( NULL == sem ) {
        fprintf(stderr, "Error: semaphore_post(): Invalid argument!\n");
        return -2;
    }

    if( NULL == sem->sem ) {
        fprintf(stderr, "Error: semaphore_post(): Semaphore has not been created yet!\n");
        return -3;
    }

    return sem_post(sem->sem);
}


int semaphore_trywait(semaphore_t *sem)
{
    /*
     * Sanity checks
     */
    if( NULL == sem ) {
        fprintf(stderr, "Error: semaphore_trywait(): Invalid argument!\n");
        return -2;
    }

    if( NULL == sem->sem ) {
        fprintf(stderr, "Error: semaphore_trywait(): Semaphore has not been created yet!\n");
        return -3;
    }

    return sem_trywait(sem->sem);
}
