#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define SIZE 1024//命令等最大的字符数
char *file;//重定向文件名
char **cmd;//保存切割完的命令参数；
char path[100][SIZE];//搜索路径，最多100个，最长SIZE
int path_mode;//标识是否经过path处理
int path_num = 1;//path的数量
int cmd_num;//分割命令得到的参数个数
typedef struct parm
{
    char *buffer;
    int m;
}parm;//多线程传递多个参数

void prin_error()//显示错误信息
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

int count(char *buffer,char c)//统计分按c割的个数
{
    int t = 0;
    char *temp = (char *)malloc(sizeof(char)*strlen(buffer)+1);//统计个数中间数组
    char *p = NULL;//切割的一个内容
    char *q=NULL;
    strcpy(temp,buffer);//复制，以免影响其他操作
    p = strtok_r(temp, &c,&q);//以c分割
    while(p)//统计个数
    {
        p = strtok_r(NULL,&c,&q);
        t++;
    }
    return t;
}

char **split_cmd(char *buffer,char c,int *num)//以c分割命令
{
    char **temp;
    char *p = NULL;//切割的一个内容
    char *q=NULL;
    int i = 0;
    *num = count(buffer,c);//以c分割命令后得到数组个数
    temp = (char **)malloc(sizeof(char*)*(*num));//开辟存取num的数组
    p = strtok_r(buffer, &c,&q);//以c分割
    while(p)
    {
        temp[i] = (char *)malloc(sizeof(char)*strlen(p)+1);//分割的p的大小
        strcpy(temp[i++],p);
        p = strtok_r(NULL, &c,&q);//以c分割
    }
    free(p);
    return temp;//返回分割后的数组
}

void run_appcmd(char *t,int update)//运行需要寻找指定路径下应用的命令
{//update为重定向标志
    char **path1;//中间路径
    int i;//查找路径的下标
    if(path_num == 0)//路径清空无法寻找，报错
    {
        prin_error();
        return;
    }
    if(path_mode == 1)//未经过path处理
    {
        path_num = 2;//未经path为2，在/bin和/usr/bin中查找
        strcpy(path[0],"/bin");
        strcpy(path[1],"/usr/bin");
    }
    path1 = (char **)malloc(sizeof(char*)*path_num);//由于查找增加应用名称,增加中间变量path1
    for(i=0;i<path_num;i++)//在path_num个路径中寻找t
    {
        path1[i] = (char *)malloc(sizeof(char)*(strlen(t)+strlen(path[i])+2));
        //中间记录路径长度动态更新为path+t的长度
        strcat(path1[i],path[i]);//添加搜索路径
        strcat(path1[i],"/");//文件夹分隔
        strcat(path1[i],t);//添加可执行的文件
        //经过以上的strcat -> path1[i] = path[i] + "/" + t
        if(!access(path1[i],X_OK))//检测是否存在且可执行
            break;
    }
    if(i>=path_num)//上面路径都没有找到则是错误
    {
        prin_error();
        return;
    }
    char *temp[cmd_num+1];//由于之前分割未加空指针，而execv需要一个空指针表示结束
    temp[cmd_num] = NULL;//空指针结束
    int status;//状态
    for(int i=0;i<cmd_num;i++)//一一复制
        temp[i] = cmd[i];
    int pid = fork();
    if(pid == -1)//创建进程失败
        prin_error();
    else if(pid == 0)//创建子进程,子进程pid=0
    {
        FILE *fp;
        if(update)//重定向标志
        {
            fp = freopen(file,"w",stdout);//打开重定向文件
            if(fp == NULL)//打开失败
            {
                prin_error();
                exit(0);//退出子进程
            }
        }
        execv(path1[i],temp);//子进程调用并执行
        fclose(fp);
    }
    else
        wait(&status);//父进程等待子进程
}

void fix_path()//修改搜索路径
{
    int i;
    if(path_mode == 1)
        path_num = cmd_num-1;//当前修改后的搜索路径个数位指令参数-1
    else if(cmd_num-1 == 0)//为0代表清空
    {
        path_mode = 0;//预防第一次进行path清空
        path_num = 0;//清空路径数为0
        return;//返回
    }
    else if(cmd_num-1 > path_num)//新的更换的路径数更大时才进行更换
        path_num = cmd_num-1;
    path_mode = 0;//进行过路径更换
    for(i = 0;i<cmd_num-1;i++)//只更新path中输入的路径数的路径
        strcpy(path[i],cmd[i+1]);//复制更新
}

void run_cmd()//执行命令
{
    if(cmd_num == 0)//参数必须要有
        prin_error();
    else if(strcmp("cd",cmd[0]) == 0)//cd命令
    {
        if(cmd_num != 2||chdir(cmd[1]))//cd参数必须有1，所以分割后参数个数必须2或者必须更换成功
            prin_error();
    }
    else if(strcmp("path",cmd[0]) == 0)//path命令
        fix_path();//路径修改
    else if(strcmp("exit",cmd[0]) == 0)//退出命令
    {
        if(cmd_num>1)//退出不含参数，分割后参数个数只能为1
            prin_error();
        else
            exit(0);
    }
    else
        run_appcmd(cmd[0],0);//包含没有的命令和直接调取可执行文件
}

void run_update(char *buffer,int m)//重定向
{
    char **c_cmd;//存取按>分割后的命令
    int c_num;
    if(m>1)//>个数只能有一个
    {
        prin_error();
        return;
    }
    c_cmd = split_cmd(buffer,'>',&c_num);//按>分割命令
    if(c_num != 2)//按>需要包含命令和文件两个
    {
        prin_error();
        return;
    }
    cmd = split_cmd(c_cmd[1],' ',&c_num);//按' '分割c_cmd[1]中的文件名
    if(c_num != 1)//重定向输出到的只能有一个
    {
        prin_error();
        return;
    }
    file = cmd[0];//file指向重定向输出到的文件
    cmd = split_cmd(c_cmd[0],' ',&cmd_num);//分割c_cmd[0]中的命令 
    if(c_num == 0)//>前面只有空格
    {
        prin_error();
        return;
    }
    run_appcmd(cmd[0],1);//运行命令，1说明需要重定向
    return;
}

void *threadrun(void *p)//建立线程的函数
{
    parm *p0 = p;//参数指向
    if(p0->m)//含有>进行重定向
        run_update(p0->buffer,p0->m);
    else//其余直接执行命令
    {
        cmd = split_cmd(p0->buffer,' ',&cmd_num);
        run_cmd();
    }
    return NULL;
}

int if_in(char *buffer,char c)//判断命令行中是否有'>' 和 '&'并统计个数
{
    int i;
    int n = 0;
    for(i=0;buffer[i]!='\0';i++)
    {
        if(buffer[i] == c)
            n++;
    }
    return n;
}

void run_more(char *buffer,int m)//输入的一行命令含有多条命令
{
    if(strcmp(buffer,"&") == 0)//只有一个&直接返回
        return;
    int b_num;// 按&分割后命令数
    int m1;//各命令中>的数量
    char **t;//指向按&分割后的命令
    t = split_cmd(buffer,'&',&b_num);//分割
    pthread_t thread[b_num];//建立的线程ID数组
    int i;//序号
    parm p1[b_num];//有含参数的线程，用于传递线程
    for(i=0;i<b_num;i++)
    {
        p1[i].m = 0;//表示没有>
        p1[i].buffer = t[i];//指向一个命令
        if((m1=if_in(t[i],'>')))//重定向
        {
            p1[i].m = m1;
        }
        if(pthread_create(&thread[i],NULL,&threadrun,&p1[i])!=0)//建立线程
            prin_error();
    }
    for(i=0;i<b_num;i++)//等待线程
    {
        if(pthread_join(thread[i],NULL)!=0)
            prin_error();
    }
}

int main(int argc,char *argv[])
{
    char buffer[SIZE];//读取的命令行
    int m;
    //m:是否含有&,>标志,mode是否是第一次读取文件
    FILE *fp;//文件指针
    path_mode = 1;//1表示没有经过path处理
    if(argc>2)//要么等待输入的wish，要么命令在一个文件批处理，argc不会大于2
    {
        prin_error();
        exit(1);//否则报错以1退出
    }
    else
    { 
        if(argc == 1)//等待输入情况
        {
            fp = stdin;//标准输入
            printf("wish> ");
        }
        else//文件中的命令
        {
            fp = fopen(argv[1],"r");//打开文件
            if(fp == NULL)//文件打开失败
            {
                prin_error();
                exit(1);
            }
        }
        while(fgets(buffer,SIZE,fp))//循环读取命令
        {
            buffer[strlen(buffer)-1] = '\0';//去掉回车
            if(strlen(buffer) == 0)//输入只有'\n'
            {
                if(argc == 1)//等待输入情况需要提示wish>
                    printf("wish> ");
                continue;
            }
            char *temp = (char *)malloc(sizeof(char)*(strlen(buffer)+1));//动态空间可回收
            strcpy(temp,buffer);//复制一下，防止检测时对后续操作造成影响
            split_cmd(temp,' ',&cmd_num);//以空格分割，看数量
            free(temp);//回收
            if(cmd_num == 0)//全为0，输入只有空格
            {
                if(argc == 1)//等待输入情况需要提示wish>
                    printf("wish> ");
                continue;
            }
            if((m=if_in(buffer,'&')))//并行命令
                run_more(buffer,m);
            else if((m=if_in(buffer,'>')))//重定向
                run_update(buffer,m);
            else//其他命令
            {
                cmd = split_cmd(buffer,' ',&cmd_num);//分割命令
                run_cmd();//运行命令
            }
            if(argc == 1)//等待输入情况需要提示wish>
                printf("wish> ");
        }
    }
}
