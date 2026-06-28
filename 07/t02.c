#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int lock_var;
time_t end_time;
sem_t sem1, sem2;

void pthread1(void *arg);
void pthread2(void *arg);

int main(int argc, char *argv[]) {
	pthread_t id1, id2;
	pthread_t mon_th_id;
	int ret;
	end_time = time(NULL) + 30;

	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 0);

	ret = pthread_create(&id1, NULL, (void *) pthread1, NULL);
	if (ret != 0)
		perror("pthread cread1");

	ret = pthread_create(&id2, NULL, (void *) pthread2, NULL);
	if (ret != 0)
		perror("pthread cread2");

	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	exit(0);
}

void pthread1(void *arg) {
	int i;
	while (time(NULL) < end_time) {
		sem_wait(&sem1);
		for (i = 0; i < 2; i++) {
			lock_var++;
			printf("lock_var=%d\n", lock_var);
			sleep(1);
		}
		sem_post(&sem2);
	}
}

void pthread2(void *arg) {
	int nolock = 0;
	int ret;
	while (time(NULL) < end_time) {
		sem_wait(&sem2);
		printf("pthread2:pthread1 got lock;lock_var=%d\n", lock_var);
		sleep(3);
		sem_post(&sem1);
	}
}