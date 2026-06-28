#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int cnt = 0;
pthread_mutex_t fastmutex = PTHREAD_MUTEX_INITIALIZER;
void task01() {
	for (int i = 0; i < 1000000; ++i) {
		pthread_mutex_lock(&fastmutex);
		++cnt;
		pthread_mutex_unlock(&fastmutex);
	}
	pthread_exit(NULL);
}

pthread_t create_task() {
	pthread_t tid01;
	int ret;
	ret = pthread_create(&tid01, NULL, (void* (*)(void *))task01, NULL);
	if (ret) {
		perror("pthread create");
		return -1;
	}
	return tid01;
}

int main() {
	pthread_t tid01, tid02;
	int ret;
	printf("main thread: %zx....\n", pthread_self());
	
	tid01 = create_task();
	tid02 = create_task();

	pthread_join(tid01, NULL);			// 等待tid01退出
	pthread_join(tid02, NULL);
	printf("global cnt = %d\n", cnt);
	return 0;
}