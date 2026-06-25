#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// 线程的任务，一次性的任务，还是循环任务
void task01(int x) {
	while (1) {
		printf("in child thread: %zx....x = %x\n", pthread_self(), x);
		sleep(1);
	}
}

int main() {
	pthread_t tid01, tid02;
	int ret;
	printf("main thread: %zx....\n", pthread_self());
	// 开始创建第一个子线程任务
	ret = pthread_create(&tid01, NULL, (void* (*)(void *))task01, (void *)100);
	if (ret) {
		perror("pthread create");
		return -1;
	}
	printf("child thread01 create success!\n");

	// 开始创建第一个子线程任务
	ret = pthread_create(&tid02, NULL, (void* (*)(void *))task01, (void *)200);
	if (ret) {
		perror("pthread create");
		return -1;
	}

	pthread_join(tid01, NULL);			// 等待tid01退出
	pthread_join(tid02, NULL);
	return 0;
}