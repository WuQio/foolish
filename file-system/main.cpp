#include "OS.h"
using namespace std;
int main()
{
    char cmd[12];
    char arg1[10], arg2[10];
    char cmdlist[12][15];
    strcpy(cmdlist[0], "my_cd");
    strcpy(cmdlist[1], "my_mkdir");
    strcpy(cmdlist[2], "my_rmdir");
    strcpy(cmdlist[3], "my_ls");
    strcpy(cmdlist[4], "my_create");
    strcpy(cmdlist[5], "my_rm");
    strcpy(cmdlist[6], "my_open");
    strcpy(cmdlist[7], "my_close");
    strcpy(cmdlist[8], "my_write");
    strcpy(cmdlist[9], "my_read");
    strcpy(cmdlist[10], "my_exitsys");
    strcpy(cmdlist[11], "my_pwd");
    startsys();
    while(1)
    {
        cout << endl;
        int i;
        scanf("%s", cmd);
        for(i=0; i<12; i++)
        {
            if(strcmp(cmd, cmdlist[i])==0)
            {
                break;
            }
        }
        if(i>11)
        {
            cout << "cmd input error!" << endl << endl;
            continue;
        }
        switch(i)
        {
            case 0:
                scanf("%s", arg1);
                my_cd(arg1);
                break;
            case 1:
                scanf("%s", arg1);
                my_mkdir(arg1);
                break;
            case 2:
                scanf("%s", arg1);
                my_rmdir(arg1);
                break;
            case 3:
                my_ls();
                break;
            case 4:
                scanf("%s", arg1);
                my_create(arg1);
                break;
            case 5:
                scanf("%s", arg1);
                my_rm(arg1);
                break;
            case 6:
                scanf("%s", arg1);
                cout << "fd==" << my_open(arg1) << endl;
                break;
            case 7:
                scanf("%s", arg1);
                my_close(atoi(arg1));
                break;
            case 8:
                scanf("%s", arg1);
                my_write(atoi(arg1));
                break;
            case 9:
                scanf("%s", arg1);
                scanf("%s", arg2);
                my_read(atoi(arg1), atoi(arg2));
                break;
            case 10:
                my_exitsys();
                cout << "----------exit system----------" << endl;
                return 0;
            case 11:
                my_pwd();
                break;
            default:
                cout << "in switch default, exit." << endl;
                return 0;
        }
    }
}