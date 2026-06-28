/* 线程临界资源访问案例
 * 在5s内完成临界资源的访问
 * 已知临界资源有var1和var2
 * 线程1对临界资源进行写入操作，线程2对临界资源进行读取操作
 * */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int var1 = 0;
int var2 = 0;
time_t end = 0;

void task1Handler(void) {
	int cnt = 0;
	while (time(NULL) < end) {
		cnt++;
		var1 = cnt;
		var2 = cnt;
	}
}

void task2Handler(void) {
	while (time(NULL) < end) {
		if (var1 != var2) {
			printf("var1 = %d, var2 = %d\n", var1, var2);
		}
	}
}


int main() {
	pthread_t tid1, tid2;
	int ret;

	end = time(NULL) + 5;

	ret = pthread_create(&tid1, NULL,
						 (void *(*)(void *))task1Handler, NULL);
	if (ret < 0) {
		perror("pthread create");
		return -1;
	}
	ret = pthread_create(&tid2, NULL,
						 (void *(*)(void *))task2Handler, NULL);
	if (ret < 0) {
		perror("pthread create");
		return -1;
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	return 0;
}
