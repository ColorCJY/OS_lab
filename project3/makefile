# 分别编译连接三个文件，注意多线程
df:
	gcc df1.c -o df1 -g -Wall -Werror -lpthread
	gcc df2.c -o df2 -g -Wall -Werror -lpthread
	gcc df3.c -o df3 -g -Wall -Werror -lpthread

# 清楚make产生的所有可执行文件
clean:
	rm df1
	rm df2
	rm df3
