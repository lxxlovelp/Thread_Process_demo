#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// 线程的任务，一次性的任务，还是循环任务
void task01(void) {
	while (1) {
		printf("in child thread: %zx....\n", pthread_self());
		sleep(1);
	}
}

int main() {
	pthread_t tid01;
	int ret;
	printf("main thread: %zx....\n", pthread_self());
	// 开始创建第一个子线程任务
	ret = pthread_create(&tid01, NULL, (void* (*)(void *))task01, NULL);
	if (ret) {
		perror("pthread create");
		return -1;
	}
	// 一旦pthread_create实现成功，此时就有2个任务，开始抢占CPU
	printf("child thread create success!\n");
	pthread_join(tid01, NULL);			// 等待tid01退出
	return 0;
}