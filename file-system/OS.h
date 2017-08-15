//
// Created by wuqiong on 17-5-29.
//

#ifndef FILE_SYSTEM_OS_H
#define FILE_SYSTEM_OS_H

#include <cstdio>
#include <cstdlib>
#include <memory.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <malloc.h>
#include <time.h>
using namespace std;

#define BLOCKSIZE 1024
#define BLOCKNUM 1000
#define DISKSIZE (BLOCKSIZE*BLOCKNUM)
#define END 65535
#define FREE 0
#define MAXOPENFILE 10
#define deslen 106

typedef struct FCB
{
    char filename[8];
    char exname[4];
    unsigned char attribute;
    struct tm time;
    unsigned short first;
    unsigned short dirno;
    unsigned long length;
    char free;
}fcb;

typedef struct FAT
{
    unsigned short id;
}fat;

typedef struct BLOCK0
{
    char information[200];
    unsigned short root;
    char *startblock;
}block0;

typedef struct USEROPEN
{
    char filename[8];
    char exname[4];
    unsigned char attribute;
    struct tm time;
    unsigned short first;
    unsigned short dirno;
    unsigned long length;
    char free;
    char dir[80];
    int count;
    char fcbstate;
    char topenfile;
}useropen;

//global variables
char *myvhard;
useropen openfilelist[MAXOPENFILE];
char currentdir[80];
char *startp;
fat* fat1;
fat* fat2;

void my_format()
{
    char *des=(char *)malloc(deslen);
    time_t timep;
    fcb *rootp;
    block0 *bootblockp;
    bootblockp=(block0 *)myvhard;
    strcpy(bootblockp->information, "10101010");
    FILE *fp=fopen("filesystem-description", "r");
    if(fp==NULL)
    {
        cout << "open filesystem-description failed." << endl;
        return;
    }
    fread(des, sizeof(char), deslen, fp);
    fclose(fp);
    strcat(bootblockp->information, des);
    //cout << "in format(), bootblockp->information is" << endl << bootblockp->information << endl;
    bootblockp->root=5;
    bootblockp->startblock=myvhard+BLOCKSIZE+BLOCKSIZE*4;
    for(int i=0; i < BLOCKNUM; i++)
    {
        if(i<5) fat1[i].id=1;
        else if(i==5)   fat1[i].id=END;
        else    fat1[i].id=FREE;
    }

    for(int i=0; i < BLOCKNUM; i++)
    {
        if(i<5) fat2[i].id=1;
        else if(i==5)   fat2[i].id=END;
        else    fat2[i].id=FREE;
    }

    rootp=(fcb *)startp;

    strcpy(rootp[0].filename, ".");
    rootp[0].exname[0]='\0';
    rootp[0].attribute=0;
    time(&timep);
    rootp[0].time=*localtime(&timep);
    rootp[0].first=5;
    rootp[0].dirno=5;
    rootp[0].length=2;
    rootp[0].free=1;

    strcpy(rootp[1].filename, "..");
    rootp[1].exname[0]='\0';
    rootp[1].attribute=0;
    time(&timep);
    rootp[1].time=*localtime(&timep);
    rootp[1].first=5;
    rootp[0].dirno=5;
    rootp[1].length=2;
    rootp[1].free=1;

//剩余fcb初始化
    for(int i=2;i<MAXOPENFILE;i++)
    {
        strcpy(rootp[i].filename,"");
        strcpy(rootp[i].exname,"");
        rootp[i].attribute=2;
        rootp[i].first=0;
        rootp[i].dirno=0;
        rootp[i].length=0;
        rootp[i].free=FREE;
    }
    free(des);
}

void startsys()
{
    myvhard=(char *)malloc(DISKSIZE);
    fat1=(fat*)myvhard+BLOCKSIZE;
    fat2=(fat*)myvhard+BLOCKSIZE*3;
    startp=myvhard+BLOCKSIZE*5;
    FILE *fp=fopen("myfsys", "rb");//只允许读和写
    if(fp)
    {
        fread(myvhard, sizeof(char), DISKSIZE, fp);
        if(strncmp(myvhard, "10101010", 8)==0)
        {
            fclose(fp);
        }
        else
        {
            cout << "----------myfsys does not exist----------" << endl;
            cout << "----------now establishing file-system----------" << endl;
            my_format();
            fwrite(myvhard, BLOCKSIZE, BLOCKNUM, fp);
            fclose(fp);
        }
    }
    else//文件不存在
    {
        cout << "----------myfsys does not exist----------" << endl;
        cout << "----------now establishing file-system----------" << endl;
        my_format();
        fp = fopen("myfsys", "wb");//"wb" 若myfsys不存在 则新建一个
        fwrite(myvhard, BLOCKSIZE, BLOCKNUM, fp);
        fclose(fp);
    }

    //初始化用户打开文件表
    for(int i=1; i < MAXOPENFILE; i++)
        openfilelist[i].topenfile=FREE;
    strcpy(openfilelist[0].filename,"\0");
    strcpy(openfilelist[0].exname,"\0");
    fcb *addr=(fcb *)startp;
    openfilelist[0].time=addr[0].time;
    openfilelist[0].attribute=0;
    openfilelist[0].first=5;
    openfilelist[0].dirno=5;//父目录所在盘块号
    openfilelist[0].length=0;
    openfilelist[0].free=1;
    strcpy(openfilelist[0].dir,"/");
    openfilelist[0].count=0;
    openfilelist[0].fcbstate=0;
    openfilelist[0].topenfile=1;
    strcpy(currentdir,"/");
}

void my_cd(char *dirname)
{
    if(strlen(dirname)>7)
    {
        cout << "dirname too long!" << endl;
        return;
    }

    int ii;//ii保存dirname目录文件在fcb表中的下标
    fcb *addr=(fcb *)(openfilelist[0].first*BLOCKSIZE+myvhard);// 当前目录文件所在的首址
    for(ii=0; ii < MAXOPENFILE; ii++)
    {
        if(strcmp(addr[ii].filename, dirname)==0)
            break;
    }
    if(ii>=MAXOPENFILE)
    {
        cout << "In my_cd(), find " << dirname << "failed." << endl;
        return ;
    }
    if(addr[ii].attribute==1)
    {
        cout << dirname << " is not a directory." << endl;
        return;
    }

    if(ii!=1)//进入子目录
    {
        strcpy(openfilelist[0].dir, currentdir);
        strcat(currentdir, addr[ii].filename);
        strcat(currentdir, "/");
    }
    else//回退到父目录
    {
        if(openfilelist[0].first!=5)
        {
            for(int i=(int)strlen(currentdir)-2; i>=0; i--)
            {
                if(currentdir[i-1]=='/')
                {
                    currentdir[i]='\0';
                    break;
                }
            }
        }

        //如果当前目录是根目录，不变
    }

    //检查openfilelist[0].fcbstate
    //增删当前目录下的文件会引起改变
    //虽然my_mkdir()和my_create()会更新
    //保险起见，再次更新
    if(openfilelist[0].fcbstate)
    {
        addr[0].length = openfilelist[0].length;
    }

    //更新openfilelist[0]里面的内容
    strcpy(openfilelist[0].filename, addr[ii].filename);
    openfilelist[0].attribute=addr[ii].attribute;
    openfilelist[0].time=addr[ii].time;
    openfilelist[0].first=addr[ii].first;
    openfilelist[0].dirno=addr[ii].dirno;
    openfilelist[0].length=addr[ii].length;
    openfilelist[0].free=addr[ii].free;
    openfilelist[0].count=0;
    openfilelist[0].fcbstate=0;
    openfilelist[0].topenfile=1;

}

int my_open(char *filename)//返回openfilelist的下标
{
    int i, j;//i是fcb下标 j是openfilelist下标
    fcb *addr=(fcb *)(openfilelist[0].first*BLOCKSIZE+myvhard);
    for (i = 1; i < MAXOPENFILE; i++)//判断该文件是否已经被打开
        if (openfilelist[i].topenfile == 1 && strcmp(openfilelist[i].filename, filename) == 0)
        {
            cout << "----------file has been open----------" << endl;
            return -1;
        }
    for(i=2; i < MAXOPENFILE; i++)//判断是否存在该文件
    {
        if(addr[i].free!=FREE && strcmp(addr[i].filename, filename)==0)
            break;
    }
    if(i >= MAXOPENFILE)
    {
        cout << filename << " does not exist. Maybe it's in another dir." << endl;
        return -1;
    }
    if(addr[i].attribute!=1)//打开非数据文件
    {
        cout << filename << " is a directory file." << endl;
        return -1;
    }
    for(j = 1; j < MAXOPENFILE; j++)
    {
        if(openfilelist[j].topenfile==FREE)
            break;
    }
    if(j >= MAXOPENFILE)//判断是否有空余的openfilelist表项
    {
        cout << "There is no more empty openfilelist item." << endl;
        return -1;
    }
    strcpy(openfilelist[j].filename, addr[i].filename);
    strcpy(openfilelist[j].exname, addr[i].exname);
    openfilelist[j].attribute=addr[i].attribute;
    openfilelist[j].time=addr[i].time;
    openfilelist[j].first=addr[i].first;
    openfilelist[j].dirno=addr[i].dirno;
    openfilelist[j].length=addr[i].length;
    openfilelist[j].free=addr[i].free;

    strcpy(openfilelist[j].dir, currentdir);
    openfilelist[j].count=0;
    openfilelist[j].fcbstate=0;
    openfilelist[j].topenfile=1;
    return j;
}

int do_read(int fd, int len, char *text)
//从读写指针开始的位置读len字节
{
    if(openfilelist[fd].topenfile==FREE)
    {
        cout << "file not open yet, cannot do_read." << endl;
        return -1;
    }
    char *buf=(char *)malloc(sizeof(char)*BLOCKSIZE);//buf相当于文件系统的内存ram
    int logic_num, off;//读写指针所在的逻辑盘块号，从0开始；块内偏移量
    int physic_num;//读写指针所在的物理盘块号
    int realreadnum=0;
    char *addr;//映射为读写指针所在磁盘块的首址
    if(buf==NULL)
    {
        cout << "In do_read(), malloc() failed." << endl;
        return -1;
    }

    if(len>(openfilelist[fd].length-openfilelist[fd].count))
    {
        len=(int)(openfilelist[fd].length-openfilelist[fd].count);
    }
    if(len==0)//读写指针已经在文件末尾
    {
        return 0;
    }

    logic_num=openfilelist[fd].count / BLOCKSIZE;
    off=openfilelist[fd].count % BLOCKSIZE;//0~1023
    physic_num=openfilelist[fd].first;
    for(int c=0; c < logic_num; c++)
    {
        physic_num=fat1[physic_num].id;
    }
    addr=myvhard+physic_num*BLOCKSIZE;
    //cout << "physic_num==" << physic_num << endl;
    //cout << "firstblk==" << openfilelist[fd].first << endl;
    for(int i = 0; i < BLOCKSIZE; i++)
    {
        buf[i]=addr[i];
        //cout << "char " << buf[i] << endl;
    }

    if(BLOCKSIZE-off >= len)
    {
        for(int i = 0; i < len; i++)
        {
            text[i]=(off+buf)[i];

        }
        realreadnum=len;
        openfilelist[fd].count+=len;
    }
    else//readblock至少为2， 分三部分读
    {
        int readblock=(len+off)/BLOCKSIZE;//要跨几个盘块读
        for(int i = 0; i < (BLOCKSIZE-off); i++)
        {
            text[realreadnum++] = (buf+off)[i];
        }
        openfilelist[fd].count += (BLOCKSIZE - off);
        physic_num=fat1[physic_num].id;

        for(int i=1; i < readblock; i++)
        {
            addr=myvhard+physic_num*BLOCKSIZE;
            for(int j = 0; j < BLOCKSIZE; j++)//先读入buf
            {
                buf[j]=addr[j];
            }
            for(int j=0; j < BLOCKSIZE; j++)
            {
                text[realreadnum++] = buf[j];
            }
            openfilelist[fd].count += BLOCKSIZE;
            physic_num=fat1[physic_num].id;
        }

        addr=myvhard+physic_num*BLOCKSIZE;
        for(int i=0; i < BLOCKSIZE; i++)
        {
            buf[i]=addr[i];
        }
        for(int i=0; i < (len+off)%BLOCKSIZE; i++)
        {
            text[realreadnum++]=buf[i];
        }
    }
    text[realreadnum]='\0';
    free(buf);
    return realreadnum;
}

void my_mkdir(char *dirname)
{
    if(strlen(dirname) > 7)
    {
        cout << "dirname too long!" << endl;
        return;
    }//判断文件名字是否超出长度范围
    if(openfilelist[0].length>MAXOPENFILE)
    {
        cout << "fcb list full, cannot mkdir." << endl;
        return;
    }//判断当前目录下是否还有空余的fcb
    fcb *addr=(fcb *)(myvhard+openfilelist[0].first*BLOCKSIZE);// . 目录文件所在首址
    int i;//当前目录文件空余fcb索引
    for(i=2; i < MAXOPENFILE; i++)
    {
        if(addr[i].free!=FREE && strcmp(addr[i].filename, dirname)==0)
        {
            cout << "file " << dirname << "already exists, cannot mkdir." << endl;
            return;
        }
    }

    for(i=2; i < MAXOPENFILE; i++)
    {
        if(addr[i].free==FREE) break;
    }

    unsigned short fatid;//新建的目录文件所分配到的盘块号
    for(fatid=6; fatid < BLOCKNUM; fatid++)
    {
        if(fat1[fatid].id==FREE)    break;
    }

    if(fatid>=BLOCKNUM)
    {
        cout << "no more free empty block." << endl;
        return ;
    }

    fat1[fatid].id=END;
    fat2[fatid].id=END;

    fcb *new_block_addr=(fcb *)(myvhard+fatid*BLOCKSIZE);//分配到的盘块首址
    strcpy(addr[i].filename, dirname);
    strcpy(addr[i].exname, "\0");
    addr[i].attribute=0;
    time_t timep;
    time(&timep);
    addr[i].time=*localtime(&timep);
    addr[i].first=fatid;
    addr[i].dirno=openfilelist[0].first;
    addr[i].length=2;
    addr[i].free=1;

    openfilelist[0].length++;
    openfilelist[0].fcbstate=1;

    addr[0].length++;//创建目录文件后 修改当前目录文件的目录项数量

    strcpy(new_block_addr[0].filename, ".");
    strcpy(new_block_addr[0].exname, "\0");
    new_block_addr[0].attribute=0;
    new_block_addr[0].time=addr[i].time;
    new_block_addr[0].first=fatid;
    new_block_addr[0].dirno=addr[i].dirno;
    new_block_addr[0].length=2;
    new_block_addr[0].free=1;

    strcpy(new_block_addr[1].filename, "..");
    strcpy(new_block_addr[1].exname, "\0");
    new_block_addr[1].attribute=0;
    new_block_addr[1].time=addr[0].time;
    new_block_addr[1].first=addr[0].first;
    new_block_addr[1].dirno=addr[0].dirno;
    new_block_addr[1].length=addr[0].length;
    new_block_addr[1].free=1;

    for(int k=2; k < MAXOPENFILE; k++)
    {
        new_block_addr[k].free=FREE;
    }
    cout << "new dir make succeed." << endl;
}

void rmdir(unsigned short blk)//传入目录文件所在盘块号
{
    fat1[blk].id=FREE;
    fat2[blk].id=FREE;

    fcb *rmdir_addr=(fcb *)(myvhard+BLOCKNUM*blk);
    for(int i=2; i <= MAXOPENFILE; i++)
    {
        if(i==(MAXOPENFILE))
        {
            fat1[rmdir_addr[0].first].id=FREE;
            fat2[rmdir_addr[0].first].id=FREE;
            return;
        }
        else if(rmdir_addr[i].free==1)
        {
            if(rmdir_addr[i].attribute==0)//是目录文件
            {
                return rmdir(rmdir_addr[i].first);
            }
            else//是数据文件
            {
                unsigned short num = rmdir_addr[i].first;
                unsigned short t;
                for(; num != END; num=t)
                {
                    t=fat1[num].id;
                    fat1[num].id=FREE;
                    fat2[num].id=FREE;
                }
            }
        }

    }
}

void my_rmdir(char *dirname)
{
    if(strlen(dirname) > 7)
    {
        cout << "dirname too long!" << endl;
        return;
    }
    int fcb_index;
    fcb *current_dir_addr=(fcb *)(myvhard+openfilelist[0].first*BLOCKSIZE);//当前目录文件所在盘块首址
    for(fcb_index=2; fcb_index < MAXOPENFILE; fcb_index++)
    {
        if(strcmp(dirname, current_dir_addr[fcb_index].filename)==0)
            break;
    }
    if(fcb_index >= MAXOPENFILE)//无该文件
    {
        cout << "file " << dirname << " not found." << endl;
        return;
    }
    if(current_dir_addr[fcb_index].attribute==1)//非目录文件
    {
        cout << "file " << dirname << " is not a directory" << endl;
        return;
    }

    rmdir(current_dir_addr[fcb_index].first);
    current_dir_addr[fcb_index].free=FREE;
    openfilelist[0].fcbstate=1;
    openfilelist[0].length--;
    return;
}

void my_ls()
{
    fcb *current_dir_addr;
    current_dir_addr=(fcb *)(myvhard+openfilelist[0].first*BLOCKSIZE);
    for(int i=0; i < MAXOPENFILE; i++)
    {
        if(current_dir_addr[i].free != FREE)
        {
            if(current_dir_addr[i].attribute==0)
                cout << current_dir_addr[i].filename << endl;
            else if(current_dir_addr[i].attribute==1 && strlen(current_dir_addr[i].exname))
                cout << current_dir_addr[i].filename << "." << current_dir_addr[i].exname << endl;
            else if(current_dir_addr[i].attribute==1 && strlen(current_dir_addr[i].exname)==0)
                cout << current_dir_addr[i].filename << endl;
        }
    }
}

int my_create(char *filename)//创建数据文件
{
    int c, ii, j;
    char name[20], exname[20];
    for(c=0; c < strlen(filename); c++)
    {
        if(filename[c]=='.')
            break;
    }
    for(ii=0; ii<c; ii++)
    {
        name[ii]=filename[ii];
    }
    name[ii]='\0';//切片文件名
    if(strlen(name) > 7)
    {
        cout << "filename too long!" << endl;
        return -1;
    }//判断文件名字是否超出长度范围

    for(j=c+1; j < strlen(filename); j++)
    {
        exname[j-c-1]=filename[j];
    }
    exname[j-c-1]='\0';//切片扩展名
    if(strlen(exname) > 3)
    {
        cout << "exname too long." << endl;
        return -1;
    }

    if(openfilelist[0].length>MAXOPENFILE)
    {
        cout << "fcb list full, cannot create file." << endl;
        return -1;
    }//判断当前目录下是否还有空余的fcb
    fcb *addr=(fcb *)(myvhard+openfilelist[0].first*BLOCKSIZE);// . 目录文件所在首址
    int i;//当前目录文件空余fcb索引
    for(i=0; i < MAXOPENFILE; i++)
    {
        if(strcmp(addr[i].filename, filename)==0 && addr[i].free)
        {
            cout << "file " << filename << " already exists, cannot create." << endl;
            return -1;
        }
    }

    for(i=2; i < MAXOPENFILE; i++)
    {
        if(addr[i].free==FREE)
            break;
    }
    if(i>=MAXOPENFILE)
    {
        cout << "in my_create(), no more free fcb." << endl;
        return -1;
    }
    unsigned short fatid;//新建的文件所分配到的盘块号
    for(fatid=6; fatid < BLOCKNUM; fatid++)
    {
        if(fat1[fatid].id==FREE)    break;
    }

    if(fatid>=BLOCKNUM)
    {
        cout << "no more free empty block." << endl;
        return -1;
    }

    fat1[fatid].id=END;
    fat2[fatid].id=END;

    strcpy(addr[i].filename, name);
    strcpy(addr[i].exname, exname);
    addr[i].attribute=1;
    time_t timep;
    time(&timep);
    addr[i].time=*localtime(&timep);
    addr[i].first=fatid;
    addr[i].dirno=openfilelist[0].first;
    addr[i].length=0;
    addr[i].free=1;

    openfilelist[0].length++;
    openfilelist[0].fcbstate=1;
    addr[0].length++;
    cout << "file create succeed." << endl;
    return 0;
}

void my_rm(char *filename)//删除数据文件
{
    int fcb_index;
    if(strlen(filename) > 7)
    {
        cout << "filename too long!" << endl;
        return ;
    }//判断文件名字是否超出长度范围
    for(int i=0; i < MAXOPENFILE; i++)//判断是否被打开
    {
        if(openfilelist[i].topenfile==1 && strcmp(openfilelist[i].filename, filename)==0)
        {
            cout << filename << " has been open, cannot rm." << endl;
            return ;
        }
    }
    fcb *addr=(fcb *)(openfilelist[0].first*BLOCKSIZE+myvhard);
    for(fcb_index=2; fcb_index < MAXOPENFILE; fcb_index++)//找要删除的文件在fcb表中的位置
    {
        if(addr[fcb_index].free!=FREE && strcmp(filename, addr[fcb_index].filename)==0)
            break;
    }
    if(fcb_index >= MAXOPENFILE)
    {
        cout << "file may not exists in current dir, maybe it was in another dir." << endl;
        return ;
    }
    unsigned short num=addr[fcb_index].first;
    unsigned short t;
    for(; num != END; num=t)
    {
        t=fat1[num].id;
        fat1[num].id=FREE;
        fat2[num].id=FREE;
    }
    addr[fcb_index].free=FREE;
}

void my_close(int fd)
{
    fcb *fatherdiraddr;
    fatherdiraddr=(fcb *)(openfilelist[fd].dirno*BLOCKSIZE+myvhard);
    if(fd >=MAXOPENFILE || fd <1)
    {
        cout << "in my_close(), openfilelist index out of range." << endl;
        return ;
    }
    if(openfilelist[fd].attribute==0)
    {
        cout << "it's a directory file." << endl;
        return ;
    }
    if(openfilelist[fd].fcbstate==1)//文件修改了，保存到目录项中
    {
        for(int i=2; i<MAXOPENFILE; i++)//找到父目录文件中 该文件的fcb 并修改
        {
            if(strcmp(openfilelist[fd].filename, fatherdiraddr[i].filename)==0)
            {
                fatherdiraddr[i].length = openfilelist[fd].length;
                break;
            }
        }
    }

    openfilelist[fd].topenfile=FREE;
    cout << "file is closed." << endl;
    return ;
}

int do_write(int fd, const char *text, char mstyle)
//返回实际写入的字节数
//w覆盖写 从读写指针的位置开始写
//a追加 从文件末尾开始写
{
    unsigned long new_len=1;//文件新长度
    int newblknum;//新盘块数量
    int oldblknum;//原盘块数量
    int delta;//盘块数量差 多减少
    oldblknum=(int)((openfilelist[fd].length%BLOCKSIZE)?(openfilelist[fd].length/BLOCKSIZE+1):(openfilelist[fd].length/BLOCKSIZE));
    if(openfilelist[fd].length==0)  oldblknum=1;//排除长度为0的情况

    if(openfilelist[fd].topenfile==FREE)
    {
        cout << "file not open yet, cannot do_write." << endl;
        return -1;
    }
    int realwritenum=0;
    char *buf=(char *)malloc(sizeof(char)*BLOCKSIZE);
    if(buf==NULL)
    {
        fprintf(stderr, "in do_write() malloc failed with %d.\n", errno);
        return -1;
    }
    int logic_num, off;//读写指针所在的逻辑盘块号，从0开始；块内偏移量
    int physic_num;//读写指针所在的物理盘块号
    char *addr;//映射为读写指针所在磁盘块的首址
    logic_num=openfilelist[fd].count / BLOCKSIZE;
    off=openfilelist[fd].count % BLOCKSIZE;//0~1023
    physic_num=openfilelist[fd].first;
    for(int c=0; c < logic_num; c++)
    {
        physic_num=fat1[physic_num].id;
    }//当count==length != 0时 physic_num==END
    addr=myvhard+physic_num*BLOCKSIZE;
    //cout << "in do_write(), ini physicnum==" << physic_num << endl;
    int blknum, temp;
    temp=(int)(strlen(text)+off);//count所在盘块开始到文件末尾的长度
    blknum=(temp%BLOCKSIZE)?(temp/BLOCKSIZE):(temp/BLOCKSIZE-1);//跨几个盘块写
    int text_index=0;

    if(mstyle=='w')//从读写指针的位置开始写
    {
        new_len=strlen(text)+openfilelist[fd].count;
        newblknum=(int)((new_len%BLOCKSIZE)?(new_len/BLOCKSIZE+1):(new_len/BLOCKSIZE));
        if(new_len >= openfilelist[fd].length)//文件新长度大于或等于原文件长度
        {
            delta=newblknum-oldblknum;
            if(delta==0)//不用加盘块
            {
                //分三部分写
                if(blknum>=1)//至少夸一个盘块写 才需要有第一部分写
                {
                    for (int i = 0; i < (BLOCKSIZE - off); i++)
                        //第一部分写
                    {
                        addr[i + off] = text[text_index++];
                    }
                    physic_num = fat1[physic_num].id;
                }

                for(int i=1; i < blknum; i++)//第二部分写
                {
                    addr=myvhard+physic_num*BLOCKSIZE;
                    for(int j=0; j < BLOCKSIZE; j++)
                    {
                        addr[j]=text[text_index++];
                    }
                    physic_num=fat1[physic_num].id;
                }
                //cout << "w >= delta==0 physic_num==" << physic_num << endl;
                addr=myvhard+physic_num*BLOCKSIZE;
                for(int i=0;i < new_len%BLOCKSIZE;i++)//第三部分写
                {
                    addr[i]=text[text_index++];
                }
            }
            else//文件长度大于原长度并且要加盘块
            {
                int lastblk=openfilelist[fd].first;//原文件最后一块盘块号
                int c, ii;//for语句中的计数
                while(fat1[lastblk].id!=END)
                {//找到原文件最后一块盘块
                    lastblk=fat1[lastblk].id;
                }
                unsigned short blk[delta];//存放fat表中申请到的新盘块号
                for(c=6, ii=0; ii<delta && c<BLOCKNUM; c++)
                {
                    if(fat1[c].id==FREE)
                    {
                        blk[ii]=(unsigned short)c;
                        ii++;
                    }
                }
                if(ii<delta)
                {
                    cout << "in do_write(), there is no more blocks in fat list." << endl;
                    return -1;
                }
                fat1[lastblk].id=blk[0];
                fat2[lastblk].id=blk[0];
                for(ii=0; ii<delta-1; ii++)
                {
                    fat1[blk[ii]].id=blk[ii+1];
                    fat2[blk[ii]].id=blk[ii+1];
                }
                fat1[blk[ii]].id=END;
                fat2[blk[ii]].id=END;
                //分配完盘块，修改完fat表

                //重新计算读写指针所在物理盘块号 考虑count==length的情况即physic_num==END
                physic_num=openfilelist[fd].first;
                for(int i=0; i < logic_num; i++)
                {
                    physic_num=fat1[physic_num].id;
                }


                //分三部分写
                if(blknum>=1)//至少夸一个盘块写 才需要有第一部分写
                {
                    for (int i = 0; i < (BLOCKSIZE - off); i++)
                        //第一部分写
                    {
                        addr[i + off] = text[text_index++];
                    }
                    physic_num = fat1[physic_num].id;
                }

                for(int i=1; i < blknum; i++)//第二部分写
                {
                    addr=myvhard+physic_num*BLOCKSIZE;
                    for(int j=0; j < BLOCKSIZE; j++)
                    {
                        addr[j]=text[text_index++];
                    }
                    physic_num=fat1[physic_num].id;
                }

                addr=myvhard+physic_num*BLOCKSIZE;
                for(int i=0;i < new_len%BLOCKSIZE;i++)//第三部分写
                {
                    addr[i]=text[text_index++];
                }
            }
        }
        else//文件新长度比原来小 此时count一定小于length
        {
            delta = oldblknum-newblknum;//盘块数量差
            if(delta)//要删盘块
            {
                int num=openfilelist[fd].first;
                for(int c=0;c < newblknum-1; c++)
                {
                    num=fat1[num].id;
                }
                //num此时为新文件最后一个盘块号
                int tmp=fat1[num].id, keep=tmp;
                fat1[num].id=END;
                fat2[num].id=END;
                //接下来删除盘块 即修改fat
                while(keep!=END)
                {
                    keep=fat1[keep].id;
                    fat1[tmp].id=FREE;
                    fat2[tmp].id=FREE;
                    tmp=keep;
                }

                //分三部分写
                if(blknum>=1)//至少夸一个盘块写 才需要有第一部分写
                {
                    for (int i = 0; i < (BLOCKSIZE - off); i++)
                        //第一部分写
                    {
                        addr[i + off] = text[text_index++];
                    }
                    physic_num = fat1[physic_num].id;
                }

                for(int i=1; i < blknum; i++)//第二部分写
                {
                    addr=myvhard+physic_num*BLOCKSIZE;
                    for(int j=0; j < BLOCKSIZE; j++)
                    {
                        addr[j]=text[text_index++];
                    }
                    physic_num=fat1[physic_num].id;
                }

                addr=myvhard+physic_num*BLOCKSIZE;
                for(int i=0;i < new_len%BLOCKSIZE;i++)//第三部分写
                {
                    addr[i]=text[text_index++];
                }
            }
            else//不用删除盘块
            {
                //new_len=openfilelist[fd].count+strlen(text);

                //分三部分写
                if(blknum>=1)//至少夸一个盘块写 才需要有第一部分写
                {
                    for (int i = 0; i < (BLOCKSIZE - off); i++)
                        //第一部分写
                    {
                        addr[i + off] = text[text_index++];
                    }
                    physic_num = fat1[physic_num].id;
                }

                for(int i=1; i < blknum; i++)//第二部分写
                {
                    addr=myvhard+physic_num*BLOCKSIZE;
                    for(int j=0; j < BLOCKSIZE; j++)
                    {
                        addr[j]=text[text_index++];
                    }
                    physic_num=fat1[physic_num].id;
                }

                addr=myvhard+physic_num*BLOCKSIZE;
                for(int i=0;i < new_len%BLOCKSIZE;i++)//第三部分写
                {
                    addr[i]=text[text_index++];
                }
            }
        }
        realwritenum=text_index;
    }
    else if(mstyle=='a')//追加写 令count==length
    {
        openfilelist[fd].count=(int)openfilelist[fd].length;
        off=openfilelist[fd].count % BLOCKSIZE;
        logic_num=openfilelist[fd].count / BLOCKSIZE;
        new_len=openfilelist[fd].length+strlen(text);
        newblknum=(int)((new_len%BLOCKSIZE)?(new_len/BLOCKSIZE+1):(new_len/BLOCKSIZE));
        delta=newblknum-oldblknum;
        blknum=newblknum-logic_num-1;//跨几个盘块写
        if(delta)//追加写 要加盘块
        {
            int lastblk=openfilelist[fd].first;//原文件最后一块盘块号
            int c, ii;//for语句中的计数
            while(fat1[lastblk].id!=END)
            {//找到原文件最后一块盘块
                lastblk=fat1[lastblk].id;
            }
            unsigned short blk[delta];//存放fat表中申请到的新盘块号
            for(c=6, ii=0; ii<delta && c<BLOCKNUM; c++)
            {
                if(fat1[c].id==FREE)
                {
                    blk[ii]=(unsigned short)c;
                    ii++;
                }
            }
            if(ii<delta)
            {
                cout << "in do_write(), there is no more blocks in fat list." << endl;
                return -1;
            }
            fat1[lastblk].id=blk[0];
            fat2[lastblk].id=blk[0];
            for(ii=0; ii<delta-1; ii++)
            {
                fat1[blk[ii]].id=blk[ii+1];
                fat2[blk[ii]].id=blk[ii+1];
            }
            fat1[blk[ii]].id=END;
            fat2[blk[ii]].id=END;
            //分配完盘块，修改完fat表

            //重新计算读写指针所在物理盘块号 考虑count==length的情况即physic_num==END
            physic_num=openfilelist[fd].first;
            for(int i=0; i < logic_num; i++)
            {
                physic_num=fat1[physic_num].id;
            }


            //分三部分写
            if(blknum>=1)//至少夸一个盘块写 才需要有第一部分写
            {
                for (int i = 0; i < (BLOCKSIZE - off); i++)
                    //第一部分写
                {
                    addr[i + off] = text[text_index++];
                }
                physic_num = fat1[physic_num].id;
            }

            for(int i=1; i < blknum; i++)//第二部分写
            {
                addr=myvhard+physic_num*BLOCKSIZE;
                for(int j=0; j < BLOCKSIZE; j++)
                {
                    addr[j]=text[text_index++];
                }
                physic_num=fat1[physic_num].id;
            }

            addr=myvhard+physic_num*BLOCKSIZE;
            for(int i=0;i < new_len%BLOCKSIZE;i++)//第三部分写
            {
                addr[i]=text[text_index++];
            }
            realwritenum=text_index;
        }
        else//追加写 不用加盘块
        {
            logic_num=openfilelist[fd].count / BLOCKSIZE;
            off=openfilelist[fd].count % BLOCKSIZE;//0~1023
            physic_num=openfilelist[fd].first;
            for(int c=0; c < logic_num; c++)
            {
                physic_num=fat1[physic_num].id;
            }
            addr=myvhard+physic_num*BLOCKSIZE;
            //以上为重新计算
            for(int i=0; i < strlen(text); i++)
            {
                addr[i+off]=text[i];
            }
            realwritenum=(int)strlen(text);
        }
    }
    openfilelist[fd].count=(int)openfilelist[fd].length;//保险起见 再次赋值
    openfilelist[fd].fcbstate=1;
    openfilelist[fd].length=new_len;
    return realwritenum;
}

int my_write(int fd)
{
    char writemod;
    string s;
    if(fd >=MAXOPENFILE || fd <1)
    {
        cout << "in my_write(), openfilelist index out of range." << endl;
        return -1;
    }
    cout << "you can write something with character 'a' or 'w'." << endl;
    cout << "\tstart from rear of document with character 'a'." << endl;
    cout << "\tstart from read-write pointer with character 'w'." << endl;
    cin >> writemod;

    if(writemod=='w' || writemod=='a')
    {
        cout << "please input the text." << endl;
        getchar();//消去输入writemod之后的空格
        getline(cin, s);
        return do_write(fd, s.data(), writemod);//do_write()函数中fcbstate已经被置1
    }
    else
    {
        cout << "write mod error, no such a mod." << endl;
        return -1;
    }
}

int my_read(int fd, int len)
{
    int ret=-2;
    if(fd >=MAXOPENFILE || fd <1)
    {
        cout << "in my_read(), openfilelist index out of range." << endl;
        return -1;
    }
    char text[len+1];
    if((ret=do_read(fd, len, text))==-1)
    {
        cout << "in my_read(), do_read failed." << endl;
        return -1;
    }
    else
    {
        cout << "read text is:" << endl;
        cout << text << endl;
        return -1;
    }
}

void my_exitsys()
{
    FILE *fp;
    fp=fopen("myfsys", "wb");
    fwrite(myvhard, BLOCKSIZE, BLOCKNUM, fp);
    fclose(fp);
    free(myvhard);
    return ;
}

void my_pwd()
{
    cout << currentdir << endl;
    return;
}
#endif //FILE_SYSTEM_OS_H
