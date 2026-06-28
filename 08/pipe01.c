#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void child_handler(int w_fd) {
	printf("ready write....\n");
	ssize_t w_len = write(w_fd, "1234", 4);
	if (w_len == -1) {
		perror("write");
		close(w_fd);
	}
	printf("write %zd bytes success\n", w_len);
	getchar();

	w_len = write(w_fd, "1234", 4);
	if (w_len == -1) {
		perror("write");
		close(w_fd);
		return ;
	}
	printf("write %zd bytes success\n", w_len);
	close(w_fd);
}

void parent_handler(int r_fd) {
	char buf[1024];

	ssize_t r_len = read(r_fd, buf, sizeof(buf) - 1);
	while (r_len) {
		if (r_len == -1) {
			perror("read");
			break;
		}
		buf[r_len] = 0;					// 如果读取的内容是字符串，做一个收尾处理
		printf("read pipe success: %zd, cnt size: %zd\n", r_len, strlen(buf));
		break;
		r_len = read(r_fd, buf, sizeof(buf) - 1);
	}
	close(r_fd);
	printf("parent read pipe exit!\n");
}

// 1. 父进程读子进程发来的消息，统计他发来的字符个数
int main() {
	int fds[2];

	int ret = pipe(fds);
	if (ret) {
		perror("pipe");
		return -1;
	}
	printf("pipe create success: %d : %d\n",fds[0], fds[1]);

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		close(fds[0]);
		close(fds[1]);
		return -1;
	}

	if (pid == 0) {
		// 子进程，具有写能力，放弃读能力
		close(fds[0]);
		child_handler(fds[1]);
		exit(0);
	} else {
		// 父进程，具有读能力，放弃写能力
		close(fds[1]);
		parent_handler(fds[0]);
	}

	// 回收子进程
	int state;
	pid_t rc;
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
			printf("child abnormal exit!\n");
		}
	}

#if 0
	ssize_t w_len = write(fds[0], "hello", 5);
	if (w_len == -1) {
		perror("write");
		return -1;
	}
#endif

	
	return 0;
}