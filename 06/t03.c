#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// 线程的任务，一次性的任务，还是循环任务
void task01(int *x) {
	int y = *x;
	while (1) {
		printf("in child thread: %zx....x = %d\n", pthread_self(), y);
		sleep(1);
	}
	pthread_exit(NULL);
}

pthread_t create_task(int flags) {
	pthread_t tid01;
	int ret;
	ret = pthread_create(&tid01, NULL, (void* (*)(void *))task01, &flags);
	if (ret) {
		perror("pthread create");
		return -1;
	}
	// printf("child thread01 create success!\n");
	return tid01;
}

int main() {
	pthread_t tid01, tid02;
	int ret;
	printf("main thread: %zx....\n", pthread_self());
	
	tid01 = create_task(100);
	tid02 = create_task(200);

	
	for (int i = 0; i < 10; ++i) {
		printf("i = %d\n", i);
		sleep(1);
	}

	pthread_join(tid01, NULL);			// 等待tid01退出
	pthread_join(tid02, NULL);
	return 0;
}