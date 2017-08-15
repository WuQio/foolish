/*
	1、页缓冲区大小：4K
	2、总缓冲区大小：64K

	1、<4K的数据立即发送，以页为单位
	2、>4K的数据，将会分成多个页的数据，分批发送。
*/
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main()
{
	int fd[2];
	int w, count = 0;
	pid_t p1, p2, p3;
	char buf[30];

	pipe(fd);
	p1=fork();

	if(p1==0)
	{
		lockf(fd[1], 1, 0);
		cout << "process1 begin writing..." << endl;
		if(write(fd[1], "process 1", 10)==-1)
		{
			cout << "process1 write failed." << endl;
			exit(1);
		}
		cout << "process1 finish writing..." << endl;
		close(fd[1]);
	}
	else 
	{
		p2=fork();
		if(p2==0)
		{
			lockf(fd[1], 1, 0);
			cout << "process2 begin writing..." << endl;
			if(write(fd[1], "process 2", 10)==-1)
			{
				cout << "process1 write failed." << endl;
				exit(1);
			}
			cout << "process2 finish writing..." << endl;
			close(fd[1]);
		}
		else 
		{
			p3=fork();
			if(p3==0)
			{
				lockf(fd[1], 1, 0);
				cout << "process3 begin writing..." << endl;
				if(write(fd[1], "process 3", 10)==-1)
				{
					cout << "process1 write failed." << endl;
					exit(1);
				}
				cout << "process3 finish writing..." << endl;
			
				int flag=fcntl(fd[1], F_GETFL);
			    fcntl(fd[1], F_SETFL, flag|O_NONBLOCK);
	
				//测试管道缓冲区的大小，保证其他内容先写
				sleep(1);
				while(1)
				{
					w=write(fd[1], "a", 1);
					if(w==-1)	break;
					count++;
				}
				cout << "count==" << count << "Bytes" << endl;
				close(fd[1]);
			}
			else 
			{
				waitpid(p1, 0, 0);
				waitpid(p2, 0, 0);
				waitpid(p3, 0, 0);
				read(fd[0], buf, 30);
				for(int i=0; i < 30; i++)
				{
					cout << buf[i];
					if(buf[i]=='\0')	cout << endl;
				}

			}
		}
	}
}
