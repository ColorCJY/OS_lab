/*观察死锁和主程序*/
//饿了就直接去拿筷子，没有的就等待且不放回
#include "df.h"
int main()
{
	pthread_t p[5]; //五个哲学家
	int a[5] = {0,1,2,3,4};//哲学家编号
	int i;//下标变量
	for (i = 0; i < 5; i++)//初始信号量1
		pthread_mutex_init(&chopstick[i],NULL);
	for(i = 0; i < 5;i++)//哲学家并行
	{
		pthread_create(&p[i],NULL,fun,&a[i]);
	}
	for(i = 0;i < 5;i++)
		pthread_join(p[i], NULL);
	return 0;
}

void *fun(void *args)//死锁
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
		pthread_mutex_lock(&chopstick[left]);//拿起左手筷子
		pthread_mutex_lock(&chopstick[right]);//拿起右手筷子
		usleep(5);//进餐
		a[phi]++;
		int ii = time(&t);
		printf("%d\t%d\t%d\tP%d\t    %d\n",1,phi,ii,phi,a[phi]);
		pthread_mutex_unlock(&chopstick[left]);//放下左手筷子
		pthread_mutex_unlock(&chopstick[right]);//放下右手筷子
	}
}
