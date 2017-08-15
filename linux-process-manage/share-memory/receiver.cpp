/*
sem_t分为有名和无名。有名的sem_t通过sem_open来创建， 而无名的sem_t通过sem_init的
初始化。 用有名的sem_t来进程间同步是件很容易的事情，百度上一搜很多想相关的例子。

有名和无名的sem_t主要区别：

1. 效率：有名sem_t是放在文件，无名的sem_t是放在内存。

2.限制：有名的sem_t可以用来同步多线程，任意多进程。而无名的sem_t可以用来同步多线
程，以及Fork出来的进程间的同步。
*/

////////当前问题，sem不能在多个进程中进行共享

#include "head.h"

using namespace std;

int main()

{
    void *shm_addr;
    char *msg;

    int shmid=shmget((key_t)SHMKEY, 128, 0666|IPC_CREAT);
    if(shmid==-1)
    {
        fprintf(stderr, "shmget() failed with:%d.\n", errno);
        exit(EXIT_FAILURE);
    }
	
    shm_addr=shmat(shmid, 0, 0);
    if(shm_addr==(void *)-1)
    {
        fprintf(stderr, "shmat() failed\n");
        exit(EXIT_FAILURE);
    }

	int semid=semget(SEMKEY, 0, 0);
	P(semid, 0);
	P(semid, 2);
	cout << "receiver waked." << endl;
	sleep(1);

    msg=(char *)shm_addr;
	V(semid, 2);
	cout << "in receiver get from sender msg == " << msg << endl;

	P(semid, 2);
	strcpy(msg, "over");
	V(semid, 2);
	sleep(1);
	cout << "in receiver 'over' has been sent" << endl;
	V(semid, 1);

    if(shmdt(shm_addr)==-1)
    {
        fprintf(stderr, "shmdt() failed\n");
        exit(EXIT_FAILURE);
    }


}
