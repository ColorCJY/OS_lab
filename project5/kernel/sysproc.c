#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
extern pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);
uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.  
  struct proc* p =myproc();
  uint64 usrpge_ptr;//待检测页表起始指针
  int npage;//待检测页表个数
  uint64 useraddr;//稍后写入结果的地址
  argaddr(0,&usrpge_ptr);//读取传来的页表起始位置
  argint(1,&npage);//读取传来的页表的数量
  argaddr(2,&useraddr);//获取传入的稍后写入结果的地址
  if(npage>64)return -1;//最大64页
  uint64 bitmap=0;//位示图记录访问过页面(0-63页)
  uint64 mask=1;//移动位数来修改bitmap进行记录
  //下面两句是为了在检测之后清楚PTE_A这一位
  uint64 complement=PTE_A;//只有PTE_A为1，PTE_A是一个标志位，在pte的第六低位
  complement=~complement;//取反
  int count=0;//页面计数
  //从页表始址开始，一次增加页面的大小，直到地址超过页表范围
  for(uint64 page =usrpge_ptr;page<usrpge_ptr+npage*PGSIZE;page+=PGSIZE)
 {pte_t* pte = walk(p->pagetable,page,0);//获取页表page的pte
 if(*pte&PTE_A)//pte的PTE_A有效
 {
   bitmap=bitmap|(mask<<count);//位示图count位记录
   *pte=(*pte)&complement;//根据提示在检测后需要清除PTE_A位
 }
  count++;//printf("bitmap:%p\n",bitmap);
}
  copyout(p->pagetable,useraddr,(char*)&bitmap,sizeof(bitmap));//将bitmap传给useraddr
  return 0;
}
#endif
uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
