#ifndef SEM_IPC_H
#define SEM_IPC_H

// 产生信号灯，用val做为初始化值，给写端初始化使用
int create_sem(const char *path, int n, int num, int val);
// 打开信号灯，给读端初始化使用
int open_sem(const char *path, int n, int num);
// 删除信号灯
void delete_sem(int sem_id, int num);

int sem_p(int semid, int sem_num);
int sem_v(int semid, int sem_num);
#endif