/*
sem_t分为有名和无名。有名的sem_t通过sem_open来创建， 而无名的sem_t通过sem_init的初始化。 用有名的sem_t来进程间同步是件很容易的事情，百度上一搜很多想相关的例子。

有名和无名的sem_t主要区别：

1. 效率：有名sem_t是放在文件，无名的sem_t是放在内存。

2.限制：有名的sem_t可以用来同步多线程，任意多进程。而无名的sem_t可以用来同步多线程，以及Fork出来的进程间的同步。
*/

////////当前问题，sem不能在多个进程中进行共享

#include "head.h"
using namespace std;

int main()
{
	void *shm_addr;
	char *msg;
	string s;
	

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

	int semid=semget(SEMKEY, 3, IPC_CREAT|0666);
	if(semid==-1)
	{
		fprintf(stderr, "semget() failed with %d.\n", errno);
		exit(EXIT_FAILURE);
	}
	union semun arg;
	arg.val=0;
	if(semctl(semid, 0, SETVAL, arg)==-1)
	{
		fprintf(stderr, "semctl() failed with %d\n.", errno);
		exit(EXIT_FAILURE);
	}
	if(semctl(semid, 1, SETVAL, arg)==-1)
    {
        fprintf(stderr, "semctl() failed with %d\n.", errno);
        exit(EXIT_FAILURE);
    }

	arg.val=1;
	if(semctl(semid, 2, SETVAL, arg)==-1)
	{
		fprintf(stderr, "semctl() failed with %d\n.", errno);
		exit(EXIT_FAILURE);
	}

	cout << "Please type something:" << endl;
	getline(cin, s);
	P(semid, 2);
	msg=(char *)shm_addr;

	strcpy(msg, s.data());
	V(semid, 0);
	V(semid, 2);
	cout << "msg has been sent to receiver." << endl;

	P(semid, 1);

	sleep(1);
	P(semid, 2);
	cout << "in sender get from receiver msg == " << msg << endl;
	V(semid, 2);
	if(shmdt(shm_addr)==-1)
	{
		fprintf(stderr, "shmdt() failed\n");  
		exit(EXIT_FAILURE);
	}

	if(shmctl(shmid, IPC_RMID, NULL)==-1)
	{
		fprintf(stderr, "rm shm failed\n");
		exit(EXIT_FAILURE);
	}

	semctl(semid, 0, IPC_RMID, arg);
	semctl(semid, 1, IPC_RMID, arg);
	semctl(semid, 2, IPC_RMID, arg);
}
