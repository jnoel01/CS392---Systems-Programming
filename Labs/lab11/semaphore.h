#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

/**
 * Definitions
 */
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
};

/**
 * Function prototypes
 */
int create_semaphore(int key);
int set_sem_value(int semid, int val);
int del_sem_value(int semid);
int P(int semid);
int V(int semid);

#endif
