#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "info.h"

static int open_shm(const char *path, int n) {
	key_t key = ftok(path, n);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	
	int shm_id = shmget(key, SHMEM_SIZE, 0666);
	if (shm_id == -1) {
		perror("shm get");
		return -1;
	}
	return shm_id;
}


msg_t *msg;
pid_t w_pid;
volatile int is_data = 0;

void reader_handler(int sig) {
	if (sig == SIGUSR1) {
		is_data = 1;
	}
}

int main() {
	int shm_id;

	signal(SIGUSR1, reader_handler);

	shm_id = open_shm(KEY_PATHNAME, KEY_INT);
	if (shm_id == -1) {
		return -1;
	}
	msg = shmat(shm_id, NULL, 0);
	// 获取写端PID信息，填写自己的信息，再通知写端
	w_pid = msg->pid;
	msg->pid = getpid();
	kill(w_pid, SIGUSR1);

	while (1) {
		while (!is_data) {
			pause();
		}
		printf("msg: %s\n", msg->text);
		is_data = 0;
		kill(w_pid, SIGUSR2);
	}
}