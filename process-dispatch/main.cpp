#include "head.h"
int main()
{
    Qnode *runprcs=NULL;
    char name='A';

    srand(time(0));
    for(int i=0; i < 26; i++)
    {
        a[i]=rand();
    }
    InQueue(&rear, name, 0);//产生第一个进程
    first=rear;
    for(int i = 1; i < 26; i++)
    {
        name++;
        InQueue(&(rear->next), name, i);
        rear=rear->next;
    }
    cout << "Processes have been produced.\n\n" << endl;
    while(first)
    {
        runprcs=OutNode();
        cout << "Choose process named " << runprcs->name << endl;

        cout << "Start to run." << endl;
        cout << "runtime==" << runprcs->runtime << "\tslice==" << runprcs->slice << endl;
        Qnode *n=first;
        int c=0;
        while(n)
        {
            c++;
            n = n->next;
        }
        int ret_cpu=-1;
        while(1)//一直运行，直到要运行的进程不是当前占据cpu的进程
        {
            if((ret_cpu=CPU(runprcs))==2)   cout << "\nstill me.\n" << endl;
            else    break;
        }
        if(ret_cpu==1)
        {
            cout << "Process's slice has been used out, out CPU.\n" << endl;
        }
        if(ret_cpu==0)
            cout << "Process run over.\n" << endl;
        //执行单个进程  并处理后备队列  若进程未完成则加入队尾
    }
    return 0;
}