#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// 系统忽略SIGQUIT信号，每隔5秒执行一次采集内容
void capture_handler(int signum) {
	printf("capture...\n");
	alarm(2);
}

int main() {
	signal(SIGQUIT, SIG_IGN);
	signal(SIGALRM, capture_handler);

	alarm(2);
	while (1) {
		pause();
	}
	return 0;
}