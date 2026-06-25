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
		// 子进程
		printf("=========================\n");
		ret = execl("/usr/bin/ls", "ls", "-l", "/opt");		// 从第一个参数里的文件 替换当前进程的上下文（代码段、数据段）
		if (ret == -1) {
			perror("execl");
			exit(-1);
		}
		printf(".............\n");
	} else {
		printf("in parent, wait child exit....\n");
		wait(NULL);
		printf("child process destroy!\n");
		getchar();
	}
	printf("parent process ready exit!\n");
	return 0;
}
















//                 ┌──────────────┐
//                 │    main()    │
//                 └──────┬───────┘
//                        │
//                        ▼
//                 fork() 创建进程
//                        │
//           ┌────────────┴────────────┐
//           │                         │
//           ▼                         ▼
//    子进程 (pid == 0)         父进程 (pid > 0)
//           │                         │
//           │                         │
//           ▼                         ▼
//  printf("================")   printf("wait child...")
//           │                         │
//           ▼                         │
//  execl("/usr/bin/ls", ...)        wait(NULL)
//           │                         │
//    ┌──────┴──────┐                  │
//    │ 成功        │                   │
//    ▼             │                 │
// ls -l /opt 执行   │                 │
// (替换子进程)      │                 │
//    │             │                 │
//    ▼             │                 ▼
// 程序结束         │        等待子进程结束
//                  │                 │
//                  ▼                 ▼
//         （execl失败才会执行）   printf("child destroyed")
//                  │
//                  ▼
//         printf("parent process ready exit")
