#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t pid;					// 为父进程服务，父进程创建子进程的进程号存储在这里
volatile int is_exit = 0;	// 为子进程服务，子进程收到退出信号，设置该标志位

void child_handler(int sig_num) {
	if (sig_num == SIGINT) {
		kill(getppid(), SIGUSR1);
	} else if (sig_num == SIGQUIT) {
		kill(getppid(), SIGUSR2);
	} else if (sig_num == SIGUSR1) {
		printf("please get off the bus\n");
		is_exit = 1;
	}
}

void parent_handler(int sig_num) {
	if (sig_num == SIGUSR1) {
		printf("let's gogogo\n");
	} else if (sig_num == SIGUSR2) {
		printf("stop the bus\n");
	} else if (sig_num == SIGTSTP) {
		kill(pid, SIGUSR1);
	}
}

int main() {
	signal(SIGINT, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	pid = fork();
	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		// 设置各自用到信号
		signal(SIGINT, child_handler);
		signal(SIGQUIT, child_handler);
		signal(SIGUSR1, child_handler);
		// 等待收到退出信息
		while (!is_exit) {
			pause();
		}
		printf("child process normal exit!\n");
		exit(0);
	} else {
		signal(SIGUSR1, parent_handler);
		signal(SIGUSR2, parent_handler);
		signal(SIGTSTP, parent_handler);
		
		wait(NULL);
		sleep(1);
		printf("parent process exited!\n");
	}
}