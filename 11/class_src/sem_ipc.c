#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int create_sem(const char *path, int n, int num, int val) {
	key_t key = ftok(path, n);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	printf("key = %x\n", key);
	
	int sem_id = semget(key, num, IPC_CREAT | 0666);
	if (sem_id == -1) {
		perror("sem get");
		return -1;
	}

	for (int i = 0; i < num; ++i) {
		semctl(sem_id, i, SETVAL, val);
	}
	return sem_id;
}

int open_sem(const char *path, int n, int num) {
	key_t key = ftok(path, n);
	if (key == -1) {
		perror("ftok");
		return -1;
	}
	printf("key = %x\n", key);
	
	int sem_id = semget(key, num, 0666);
	if (sem_id == -1) {
		perror("sem get");
		return -1;
	}
	return sem_id;
}

void delete_sem(int sem_id, int num) {
	for (int i = 0; i < num; ++i) {
		int ret = semctl(sem_id, i, IPC_RMID);
		if (ret == 0) {
			printf("sem delete success!\n");
		}
	}
}

// P 操作：锁定（Wait）
int sem_p(int semid, int sem_num) {
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;          // -1 代表 P 操作
    sb.sem_flg = SEM_UNDO;   // 进程退出时自动释放，防止死锁
    return semop(semid, &sb, 1); 
}

// V 操作：解锁（Signal）
int sem_v(int semid, int sem_num) {
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;           // +1 代表 V 操作
    sb.sem_flg = SEM_UNDO;
    return semop(semid, &sb, 1);
}