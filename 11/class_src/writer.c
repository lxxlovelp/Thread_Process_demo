#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sem_ipc.h"
#include "info.h"
int shm_id;
int sem_id;
// 写端，负责创建共享内存，由于共享内存是临界资源，同时使用信号灯实现互斥操作
static int create_shm(const char *path, int n) {
	key_t key = ftok(path, n);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	
	int shm_id = shmget(key, SHMEM_SIZE, IPC_CREAT | 0666);
	if (shm_id == -1) {
		perror("sem get");
		return -1;
	}
	return shm_id;
}

static void del_shm(int shm_id) {
	int ret = shmctl(shm_id, IPC_RMID, NULL);
	if (ret == -1) {
		perror("shmctl del");
	}
}

void exit_handler(int sig) {
	del_shm(shm_id);
	delete_sem(sem_id, 1);
	exit(0);
}

// 0 ---> 有资源		1 ---> 没资源
int main() {
	shm_id = create_shm(KEY_PATHNAME, KEY_INT);
	if (shm_id == -1) {
		return -1;
	}
	sem_id = create_sem(KEY_PATHNAME, KEY_INT, 2, 1);
	if (sem_id == -1) {
		return -1;
	}
	semctl(sem_id, 1, SETVAL, 0);

	signal(SIGINT, exit_handler);
	char *msg = shmat(shm_id, NULL, 0);

	while (1) {
		sem_p(sem_id, 0);
		printf("input content:");
		fflush(stdout);
		fgets(msg, SHMEM_SIZE, stdin);
		msg[strlen(msg) - 1] = 0;			// 剔除\n符号
		sem_v(sem_id, 1);
	}
}