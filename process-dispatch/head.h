//
// Created by wuqiong on 17-6-3.
//

#ifndef PROCESS_DISPATCH_HEAD_H
#define PROCESS_DISPATCH_HEAD_H
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

typedef struct node
{
    char name;
    int runtime;//10000~100000
    int waittime;
    int s_prio;
    int prio;
    int slice;
    struct node* next;
}Qnode;

Qnode *first=NULL;
Qnode *rear=NULL;
int a[26];

void InQueue(Qnode **curr, char name, int i);
Qnode *FindMax();
Qnode *OutNode();
void DealWaitprcs(int time);
int CPU(Qnode *prcs);


void InQueue(Qnode **curr, char name, int i)//curr为队尾指针的next
{
    (*curr)=(Qnode *)malloc(sizeof(Qnode));
    (*curr)->name=name;
    (*curr)->runtime=a[i]%90001+10000;
    (*curr)->waittime=0;
    (*curr)->s_prio=a[i]%10+15;
    (*curr)->prio=(*curr)->s_prio;
    (*curr)->slice=((*curr)->s_prio)*1000;
    (*curr)->next=NULL;
}

Qnode *FindMax()
//找进程 被OutNode函数调用
{
    Qnode *ret=first;
	if(first==NULL) return NULL;
	if(first->next==NULL)	return first;
    Qnode *node=first->next;
    while(node)
    {
        if(node->prio > ret->prio)
            ret=node;
        if(node->prio == ret-> prio && node->waittime > ret->waittime)
            ret=node;
        node = node -> next;
    }
    return ret;
}

Qnode *OutNode()
//找到一个动态优先级最大的进程 出列 返回其指针
{
    Qnode *ret = FindMax();
    Qnode *last=first;
    if(ret != first)
    {
        while(last->next != ret)
            last=last->next;
        last->next = ret->next;
        if(ret==rear)
            rear=last;
    }else{
        first=first->next;
        ret->next=NULL;
    }
    return ret;
}

void DealWaitprcs(int time)
{
    Qnode *tmp=first;
    while(tmp)
    {
        tmp->waittime+=time;
        tmp->prio = tmp->s_prio + tmp->waittime/10000;
        //进程时间片只与静态优先级有关 无需改变
        tmp=tmp->next;
    }
}

int CPU(Qnode *prcs)
{
    if(prcs->slice >= prcs->runtime)//一次可以运行完成
    {
        for(int i=prcs->runtime; i>=0; i--);//模拟cpu运行

        DealWaitprcs(prcs->runtime);

        free(prcs);
        return 0;
    }
    else//本次运行不能完成
    {
        for(int i = prcs->slice; i>=0; i--);//模拟cpu运行

        DealWaitprcs(prcs->slice);

        prcs->runtime -= prcs->slice;
        prcs->waittime=0;
        prcs->prio-=(prcs->slice/10000);
        Qnode *tmp = FindMax();
        if(tmp)
            if(tmp->prio < prcs->prio)
            {
                return 2;//cpu中的进程优先级仍然最高
            }else{
                rear->next = prcs;
                prcs->next=NULL;
                rear=prcs;
                return 1;//slice用完且要退出cpu
            }
        else//队列空了
            return 2;
    }
}

#endif //PROCESS_DISPATCH_HEAD_H
