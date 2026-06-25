#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void my_func();
volatile int i;

int main()
{
	pthread_t my_thread;
	int err,k;
	i = 2;
	if((err = pthread_create(&my_thread,NULL,(void *)my_func,NULL)) < 0)
		perror("can't create thread:%s\n");
	
	while(i == 2);
	printf("main:%d\n",i);
	while(1) { sleep(1);}
	return 0;
}

void my_func()
{
	sleep(1);
	i = 3;
	printf("my_func:%d\n",i);
}

