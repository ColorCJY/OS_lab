#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/date.h"//给出一个日期,计算这是本年中第几天
int
main(int argc, char *argv[])
{
 int sum,flag;
  struct rtcdate p;
  p.year=2021;p.month=10;p.day=10;
switch(p.month)/*先计算某月以前月份的总天数*/
{
case 1:sum=0;break;
case 2:sum=31;break;
case 3:sum=59;break;
case 4:sum=90;break;
case 5:sum=120;break;
case 6:sum=151;break;
case 7:sum=181;break;
case 8:sum=212;break;
case 9:sum=243;break;
case 10:sum=273;break;
case 11:sum=304;break;
case 12:sum=334;break;
default:printf("日期输入错误\n");break;
}
sum=sum+p.day;/*再加上某天的天数*/
if(p.year%400==0||(p.year%4==0&&p.year%100!=0))/*判断是不是闰年*/
flag=1;
else
flag=0;
if(flag==1&&p.month>2)/*如果是闰年且月份大于2,总天数应该加一天*/
sum++;
printf("%d年%d月%d日是%d的%d天\n",p.year,p.month,p.day,p.year,sum);
  
  exit(0);
}
