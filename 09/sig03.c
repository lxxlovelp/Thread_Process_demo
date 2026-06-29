#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pid;

// 用户发出Ctrl+c，父进程收到，向子进程发送SIGUSR1信号，子进程才退出，父进程继续做别的事情
void fun(int signum)
{
	if (signum == SIGINT) {
		printf("ready to send signal...\n");
		kill(pid, SIGUSR1);
	} else if (signum == SIGUSR1) {
		printf("clear child process....\n");
	} else if (signum == SIGCHLD) {
		wait(NULL);
	} else {
		printf("No matter!\n");
	}
}

int main()
{
	int ret;
	signal(SIGINT,SIG_IGN);
	signal(SIGUSR1,SIG_IGN);

	if((pid=fork())<0){
		perror("fork");
		exit(1);
	}
	if(pid == 0){
		signal(SIGUSR1,fun);
		pause();
		printf("the child process exit!\n");
		exit(0);
	}
	else{
		signal(SIGINT,fun);
		signal(SIGCHLD, fun);
		while (1) {
			pause();
			printf("parent running...\n");
		}
		
#if 0
		if((waitpid(pid,NULL,WNOHANG))==0)
		{
			if((ret=kill(pid,SIGINT))==0)
			//if((ret=kill(pid,SIGKILL))==0)
				printf("kill %d\n",pid);
			else{
				perror("kill");
			}
		}
		getchar();
#endif
	}
}