#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * 子进程处理函数（写管道）
 * w_fd：管道写端
 */
void child_handler(int w_fd) {
	printf("ready write....\n");

	// 向管道写入 4 个字节
	ssize_t w_len = write(w_fd, "1234", 4);
	if (w_len == -1) {
		perror("write");
		close(w_fd);
	}

	printf("write %zd bytes success\n", w_len);

	// 阻塞一下，让你观察管道行为（模拟进程暂停）
	getchar();

	// 第二次写入
	w_len = write(w_fd, "1234", 4);
	if (w_len == -1) {
		perror("write");
		close(w_fd);
		return;
	}

	printf("write %zd bytes success\n", w_len);

	// 关闭写端（很重要：通知读端 EOF）
	close(w_fd);
}

/*
 * 父进程处理函数（读管道）
 * r_fd：管道读端
 */
void parent_handler(int r_fd) {
	char buf[1024];

	// 从管道读取数据
	ssize_t r_len = read(r_fd, buf, sizeof(buf) - 1);

	// 只执行一次（这里 break 导致循环失效）
	while (r_len) {

		// 读取失败
		if (r_len == -1) {
			perror("read");
			break;
		}

		// 添加字符串结束符（因为 read 不会自动加 '\0'）
		buf[r_len] = 0;

		// 输出读取结果
		printf("read pipe success: %zd, cnt size: %zd\n",
		       r_len, strlen(buf));

		// ⚠️ 注意：这里 break 导致只读一次
		break;

		// 如果不 break，本意是循环读
		r_len = read(r_fd, buf, sizeof(buf) - 1);
	}

	// 关闭读端
	close(r_fd);

	printf("parent read pipe exit!\n");
}

/*
 * 主函数：创建 pipe + fork
 */
int main() {

	int fds[2];  // fds[0] = 读端, fds[1] = 写端

	// 创建匿名管道
	int ret = pipe(fds);
	if (ret) {
		perror("pipe");
		return -1;
	}

	printf("pipe create success: %d : %d\n", fds[0], fds[1]);

	// 创建子进程
	pid_t pid = fork();

	if (pid == -1) {
		perror("fork");
		close(fds[0]);
		close(fds[1]);
		return -1;
	}

	/*
	 * 子进程逻辑
	 * 只写，不读
	 */
	if (pid == 0) {

		// 关闭读端（子进程不用读）
		close(fds[0]);

		// 处理写入
		child_handler(fds[1]);

		// 子进程退出
		exit(0);

	} else {

		/*
		 * 父进程逻辑
		 * 只读，不写
		 */

		// 关闭写端（父进程不用写）
		close(fds[1]);

		// 读取管道
		parent_handler(fds[0]);
	}

	/*
	 * 父进程回收子进程
	 */
	int state;
	pid_t rc = wait(&state);

	if (rc == -1) {
		perror("wait");
		exit(0);
	}

	// 判断回收的是不是目标子进程
	if (rc == pid) {
		printf("catch child process exit state: %x!\n", state);

		// 正常退出
		if (WIFEXITED(state)) {
			printf("child process normal exit: %d\n",
			       WEXITSTATUS(state));
		}
		// 异常退出
		else {
			printf("child abnormal exit!\n");
		}
	}

	return 0;
}