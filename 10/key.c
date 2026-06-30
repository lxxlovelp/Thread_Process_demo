#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// 3. 自定义一个消息类型 1 + x
struct msgbuf {
	long mtype;
	char mtext[32];
};

int main() {
	// 1. 生成key值
	key_t key = ftok("/tmp", 556);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	printf("key = %x\n", key);
	// 2. 通过key值获取、创建 IPC对象，创建时，增加0666的权限位
	int msg_fd = msgget(key, IPC_CREAT | 0666);
	if (msg_fd == -1) {
		perror("msg get");
		return -1;
	}
	printf("get msgQueue id: %x\n", msg_fd);

	// 4. 定义消息类型的消息
	struct msgbuf msg;
	msg.mtype = 200;
	snprintf(msg.mtext, sizeof(msg.mtext), "4567");
	int ret = msgsnd(msg_fd, &msg, strlen(msg.mtext), 0);
	if (ret == -1) {
		perror("msg snd");
		return 0;
	}

	return 0;
}