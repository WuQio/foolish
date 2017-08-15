#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEMKEY 0x100
#define SHMKEY 0x200

int P(int semid, int semnum)
{
        struct sembuf sops={semnum,-1, SEM_UNDO};
        return (semop(semid,&sops,1));
}
int V(int semid, int semnum)
{
        struct sembuf sops={semnum,+1, SEM_UNDO};
        return (semop(semid,&sops,1));
}
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
