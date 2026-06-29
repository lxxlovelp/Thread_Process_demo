#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sig_handler(int sig) {
	if (sig == SIGINT) {
		printf("Catch a SIGINT singal!\n");
	} else if (sig == SIGQUIT) {
		printf("Catch a SIGQUIT singal!\n");
	} else if (sig == 39) {
		printf("Catch a SIGRTMIN+5 singal!\n");
	} 
	else {
		printf("Other signal!\n");
	}
}

int main() {
	// 1. 注册异步处理的接口到框架
	signal(SIGINT, sig_handler);
	// signal(SIGQUIT, SIG_IGN);
	signal(39, sig_handler);

	// 2. 等待事件信号的发生
	while (1) {
		pause();
		printf("in main process!\n");
	}
	return 0;
}