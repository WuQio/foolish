#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
using namespace std;

struct msg
{
	long mtype;
	char mtext[512];
};

int mq;
sem_t sem, sem2, use;
void *f_sender(void *)
{
	int ret_msgsnd;
	string str;
	struct msg snd, rcv;
	snd.mtype=1;//防止msgsnd报错
	int sendlength=sizeof(struct msg)-sizeof(long);
	while(getline(cin, str), str!="exit")
	{
		sem_wait(&use);
		strcpy(snd.mtext, str.data());
		ret_msgsnd=msgsnd(mq, &snd, sendlength, 0);
		sem_post(&sem);
		if(ret_msgsnd==-1)
		{
			fprintf(stderr, "message send error! with :%d\n", errno);
		}
		cout << "in thread sender, send: " << snd.mtext << endl;;
		sem_post(&use);
	}
	
	sem_wait(&use);
	strcpy(snd.mtext, str.data());
	if(msgsnd(mq, &snd, sendlength, 0)==-1)	fprintf(stderr, "message exit send error! with :%d\n", errno);
	sem_post(&sem);
	cout << "in thread sender, send: " << snd.mtext << endl;
	sem_post(&use);

	sem_wait(&use);
	strcpy(snd.mtext, "end");
	if(msgsnd(mq, &snd, sendlength, 0)==-1)	fprintf(stderr, "message end send error! with :%d\n", errno);
	sem_post(&sem);
	cout << "in thread sender, send: " << snd.mtext << endl;
	sem_post(&use);
//等待应答

	sem_wait(&sem2);
	sem_wait(&use);
	msgrcv(mq, &rcv, sendlength, 0, /*IPC_NOWAIT*/0);
	sem_post(&use);
	cout << "in thread sender, receive: " << rcv.mtext << endl;
	sem_destroy(&sem);
	cout << "semaphore sem destroyed." << endl;
	sem_destroy(&sem2);
	cout << "semaphore sem2 destroyed." << endl;
	sem_destroy(&use);
	cout << "semaphore use destroyed." << endl;
}

void *f_receive(void *)
{
	struct msg snd, rcv;
	while(1)
	{
		sem_wait(&sem);
		sem_wait(&use);
		msgrcv(mq, &rcv, sizeof(struct msg)-sizeof(long), 0, 0);
		cout << "in thread receive, get: " << rcv.mtext << endl;
		sem_post(&use);
		if(strncmp(rcv.mtext, "end", 3)==0)
		{
			break;
		}
	}
	sem_wait(&use);
	strcpy(snd.mtext, "over");
	snd.mtype=1;
	if(msgsnd(mq, &snd, sizeof(struct msg)-sizeof(long), 0))
	{
		fprintf(stderr, "message over send error! with :%d\n", errno);
	}
	sem_post(&sem2);
	sem_post(&use);
	cout << "in thread receive, send: " << snd.mtext << endl;
}


int main()
{
	pthread_t sender, receive;
	mq=msgget((key_t)1234, 0666|IPC_CREAT);
    if(mq==-1)
    {
        cout << "msgget() failed." << endl;
        fprintf(stderr, "msgget() error! with :%d\n", errno);
        exit(1);
    }
	sem_init(&sem, 0, 0);
	sem_init(&sem2, 0, 0);
	sem_init(&use, 0, 1);
	int s=pthread_create(&sender, NULL, f_sender, NULL);
	int r=pthread_create(&receive, NULL, f_receive, NULL);

	if(s|r)
	{
		cout << "Thread creating error!" <<endl;
		exit(1);
	}
	
	if(msgctl(mq, IPC_RMID, NULL)<0)
	{
		cout << "msgctl() faield." << endl;
		return -1;
	}
	pthread_join(sender, 0);
	pthread_join(receive, 0);
}
