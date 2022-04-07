/*解决死锁1*/
//尝试拿起左手筷子后判断右手，不能拿起放下左手，继续尝试
//否则吃饭，再同时放下
#include "df.h"
int main()
{
	pthread_t p[5];//五个哲学家
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

void *fun(void *args)//解决死锁1
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
        if (pthread_mutex_trylock(&chopstick[right]) != 0)//尝试拿起右手的筷子,不为0筷子被拿走无法拿起
        {	
			pthread_mutex_unlock(&chopstick[left]); //如果右边筷子被拿走放下左手的筷子
			continue;
		}
		usleep(5);//进餐
		a[phi]++;
		int ii = time(&t);
		printf("%d\t%d\t%d\tP%d\t    %d\n",2,phi,ii,phi,a[phi]);
		//放下左右手筷子
		pthread_mutex_unlock(&chopstick[left]);
		pthread_mutex_unlock(&chopstick[right]);
    }
}
