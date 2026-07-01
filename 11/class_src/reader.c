#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include "sem_ipc.h"
#include "info.h"

// 写端，负责创建共享内存，由于共享内存是临界资源，同时使用信号灯实现互斥操作
static int open_shm(const char *path, int n) {
	// 生成一个唯一的key
	key_t key = ftok(path, n);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	// 打开共享内存
	int shm_id = shmget(key, SHMEM_SIZE, 0666);
	if (shm_id == -1) {
		perror("shm get");
		return -1;
	}
	return shm_id;
}

char *msg = NULL;// flag 里面是随机垃圾值它可能指向任意内存地址,不太理解

// 删除共享内存
static void del_shm(int shm_id) {
	int ret = shmctl(shm_id, IPC_RMID, NULL);
	if (ret == -1) {
		perror("shmctl del");
	}
}

// 信号处理函数，捕获Ctrl+C信号，解除共享内存映射
void exit_handler(int sig) {
	shmdt(msg);
}

int main() {
	int shm_id;
	int sem_id;
	shm_id = open_shm(KEY_PATHNAME, KEY_INT);
	if (shm_id == -1) {
		return -1;
	}
	sem_id = open_sem(KEY_PATHNAME, KEY_INT, 1);
	if (sem_id == -1) {
		return -1;
	}

	signal(SIGINT, exit_handler);
	msg = shmat(shm_id, NULL, 0);

	while (1) {
		sem_p(sem_id, 1);
		printf("msg: %s\n", msg);
		sem_v(sem_id, 0);
	}
}