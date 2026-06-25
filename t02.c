#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	pid_t pid;
	int ret;
	pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		printf("child process running...\n");
		getchar();
		printf("child process normal exit...\n");
		exit(5);
	} else {
		// 等待子进程退出
		int state;
		pid_t rc;
		printf("parent process wait...!\n");
		rc = wait(&state);
		if (rc == -1) {
			perror("wait");
			exit(0);
		}
		if (rc == pid) {
			printf("catch child process exit state: %x!\n", state);
			if (WIFEXITED(state)) {
				printf("child process normal exit: %d\n", WEXITSTATUS(state));
			} else {
				printf("child normal exit!\n");
			}
		}

	}
	printf("parent process ready exit!\n");
	return 0;
}