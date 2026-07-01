#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "info.h"

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

msg_t *msg;
volatile int start_flag = 0;
pid_t client_pid;

void write_handler(int sig) {
	if (sig == SIGUSR1) {
		client_pid = msg->pid;
		start_flag = 1;
	} else if (sig == SIGUSR2) {
		start_flag = 1;
	}
}

int main() {
	int shm_id = create_shm(KEY_PATHNAME, KEY_INT);
	if (shm_id == -1) {
		return -1;
	}

	msg = shmat(shm_id, NULL, 0);
	msg->pid = getpid();

	// 信号处理注册
	signal(SIGUSR1, write_handler);
	signal(SIGUSR2, write_handler);
	
	while (1) {
		while (!start_flag) {
			pause();
		}
		printf("input content:");
		fflush(stdout);
		fgets(msg->text, SHMEM_SIZE, stdin);
		msg->text[strlen(msg->text) - 1] = 0;			// 剔除\n符号
		start_flag = 0;
		kill(client_pid, SIGUSR1);
	}
}