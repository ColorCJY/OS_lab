/*解决死锁2*/
//最多允许四位哲学家同时拿起左手(或右手)筷子
#include "df.h"
#include <semaphore.h>
/*P,v操作*/
#define P sem_wait
#define V sem_post

sem_t mutex;//限制4名信号量

int main()
{
	pthread_t p[5];//五个哲学家
	int a[5] = {0,1,2,3,4};//哲学家编号
	int i;//下标变量
	for (i = 0; i < 5; i++)//初始信号量1
		pthread_mutex_init(&chopstick[i],NULL);
	sem_init(&mutex, 0, 4);//信号量初始为4，限制四名
	for(i = 0; i < 5;i++)//哲学家并行
	{
		pthread_create(&p[i],NULL,fun,&a[i]);
	}
	for(i = 0;i < 5;i++)
		pthread_join(p[i], NULL);
	return 0;
}

void *fun(void *args)//解决死锁2
{
	int phi = *(int *)args;//获得哲学家编号
	int left = phi; //左手筷子序号
	int right = (phi+4) % 5;//右手筷子序号
	time_t t;
	t = time(NULL);
	if(phi == 0)
		printf("pid\ttid\t   time        P_num     eat_num\n");
	while(1)
	{
		usleep(3);//思考
		P(&mutex);//最多允许四位哲学家申请
		pthread_mutex_lock(&chopstick[left]);//拿起左手筷子
		pthread_mutex_lock(&chopstick[right]);//拿起左手筷子
		V(&mutex);//已拿到解除申请
		usleep(5);//进餐
		a[phi]++;
		int ii = time(&t);
		printf("%d\t%d\t%d\tP%d\t    %d\n",3,phi,ii,phi,a[phi]);
		//放下左右手筷子
		pthread_mutex_unlock(&chopstick[left]);
		pthread_mutex_unlock(&chopstick[right]);
	}
}
