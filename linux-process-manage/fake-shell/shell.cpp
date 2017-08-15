#include <iostream>
#include <cstdio>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;
void printLoginName()
{
	string tmp;
	struct passwd *pass;
	pass=getpwuid(getuid());
	cout << pass->pw_name;
}

void printHostName()
{
	char name[128];
	gethostname(name, sizeof(name)-1);
	cout << name;
}

void printDir()
{
	char dir[128];
	char *homedir;
	getcwd(dir, sizeof(dir)-1);
	homedir = getenv("HOME");
	string Sdir(dir);
	string Shomedir(homedir);
	/* 
	* function string.find(sub)
	* returns the index of sub's first character in string if find
	* returns string::npos if not find
	*/
	if(Sdir.find(homedir) != string::npos)	cout << "~" << Sdir.substr(Shomedir.length());
	else cout << dir;
}
int main()
{
	pid_t pid;
    int status;
    const char *cmdstring;
    string s;
	int flag;

	loop:flag=0;
	cout << "[";
	printLoginName();
	cout << "@";
	printHostName();
	cout << ":";
	printDir();
	cout << "]";
	if(geteuid())	cout << "$ ";
	else	cout << "# ";

    getline(cin, s);
    cmdstring = s.data();
	if(s.substr(0, 2)=="cd")	flag=1;
	if(s.substr(0, 4)=="exit")	flag=2;
    if((pid = fork()) < 0)
    {
        cout << "fork error!" << endl;
    }
    else if(pid==0)//从子进程返回
    {
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		exit(127);
    }
    else//从父进程返回
    {
        waitpid(pid, &status, 0);
		if(flag==1)	{chdir(s.substr(3).data());}
		if(flag==2)	{return 0;}
        goto loop;
    }

}
