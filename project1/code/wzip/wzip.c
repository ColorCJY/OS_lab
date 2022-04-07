#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) 
{
	FILE* fp; //文件指针
  	char c = 0; //读取的字符
  	char temp = 0;  //记录匹配的字符
  	int num = 0;  //字符计数
	if (argc < 2) // 当输入的参数只有一个时，提示没有输入文件
	{
    	printf("wzip: file1 [file2 ...]\n");
    	exit(1);
  	}
  	for (int i = 1; i < argc; i++) //一个或多个文件
  	{
    	fp = fopen(argv[i],"r");//打开要压缩的文件
    	if (fp == NULL) //打开失败
    	{
    		printf("wzip: cannot open file\n");
   			exit(1);
  		}
    	while ((c = fgetc(fp)) != EOF) //在一个文件中一个一个字符的读
    	{
      		if (temp == 0) //读取第一个字符
      		{
        		temp = c; //记录
        		num++; //数量+1
      		} 
      		else if (c != temp) //当不相等 
      		{
       		 	fwrite(&num, sizeof(int), 1, stdout); //标准输出到文件，二进制
        		fputc(temp, stdout); //输出字符
        		num=1; //重新置新字符的个数
      		} 
      		else 
      		{
        		num++;//相同数量+1
      		}
      		temp = c;//重新置待匹配的字符
    	}
  	}
  	//读到文件末时退出但此时还有一种字符没被写入
    fwrite(&num, sizeof(int), 1, stdout);
    fputc(temp, stdout);
  	return 0;
}
