#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 20 //一次读取buffer的最大字符数
int main(int argc,char *argv[])
{
	int i = 1;//打开第i个文件
	FILE *fp; // 文件指针
	char buffer[MAX_SIZE]; //存取一次读取的字符
	for(;i<argc;i++)
	{	
		fp = fopen(argv[i],"r");//打开文件
		if(fp == NULL) // 打开失败
		{
			printf("wcat: cannot open file\n");
			exit(1);
		}
		while(fgets(buffer,MAX_SIZE,fp)) //循环读取数据
			printf("%s",buffer); //显示读取到文件中的字符
	}
	return 0;
}

