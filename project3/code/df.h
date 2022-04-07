/*定义dp头文件，包含所需库，信号量以及相关函数*/
#ifndef DF_H
#define DF_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t chopstick[6];
int a[5] = {0};

void *fun(void *args);//功能程序

#endif