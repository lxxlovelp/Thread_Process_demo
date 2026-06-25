#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
/* 临界资源访问
 * 访问临界资源的时候，会出现访问互斥情况
 * */
int value1 = 0;
int value2 = 0;
int count = 0;
pthread_mutex_t lock;

#ifdef UNDEAD
void cleanup_handler() {
	pthread_mutex_unlock(&lock);
}
#endif

void thread1_handler() {
	while (1) {
		pthread_mutex_lock(&lock);
#ifdef UNDEAD
		pthread_cleanup_push(cleanup_handler, NULL);
#endif
		value1 = count;
		value2 = count;
		count++;
		sleep(3);
#ifdef UNDEAD
		pthread_cleanup_pop(NULL);
#endif
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(0);
}

void thread2_handler() {
	while (1) {
		sleep(1);
		printf("thread try lock...!\n");
		pthread_mutex_lock(&lock);
		printf("thread locked...!\n");
		if (value1 == value2) {
			sleep(1);
			printf("value1 = %d, value2 = %d\n", value1, value2);
		}
		pthread_mutex_unlock(&lock);
	}
	pthread_exit("thread2...exit");
}

int main() {
	pthread_t thread1, thread2;

	if (pthread_mutex_init(&lock, NULL)) {
		perror("mutex init");
		return -1;
	}

	if (pthread_create(&thread1, NULL,
					   (void *(*)(void *))thread1_handler, NULL)) {
		perror("create thread1");
		return -1;
	}
	if (pthread_create(&thread2, NULL,
					   (void *(*)(void *))thread2_handler, NULL)) {
		perror("create thread1");
		return -1;
	}
	sleep(2);
	pthread_cancel(thread1);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_mutex_destroy(&lock);

	return 0;
}
