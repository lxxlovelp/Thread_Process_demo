#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
	// 1. 生成key值
	key_t key = ftok("/tmp", 556);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	printf("key = %x\n", key);
	// 2. 通过key值获取、创建 IPC对象，创建时，增加0666的权限位，物理空间申请
	int shm_id = shmget(key, 0x1<<12, IPC_CREAT | 0666);
	if (shm_id == -1) {
		perror("shm get");
		return -1;
	}
	printf("get shm id: %x\n", shm_id);

	// 3. 对申请的物理空间做一个映射，在当前进程的虚拟地址 映射到 该物理地址
	char *addr = shmat(shm_id, NULL, 0);
	if (addr == (char *)-1) {
		perror("shmat");
		return -1;
	}
	printf("at success!\n");
	addr[0] = 0x31;
	addr[1] = 0x32;

	// 4. 不用，dt
	shmdt(addr);
	return 0;
}