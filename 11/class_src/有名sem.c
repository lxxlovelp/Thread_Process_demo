#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>           /* 用于 O_555 等常量 */
#include <sys/stat.h>        /* 用于 S_IRUSR 等常量 */
#include <semaphore.h>
#include <unistd.h>

int main() {
    // 1. 创建共享内存
    key_t key = ftok(".", 'x');
    int shmid = shmget(key, 1024, IPC_CREAT | 0666);
    char *shm_ptr = shmat(shmid, NULL, 0);

    // 2. 创建并初始化有名信号量，初始值设为 1（互斥锁）
    // O_CREAT: 如果不存在就创建；0666: 权限；1: 初始值
    sem_t *sem = sem_open("/my_shm_sem", O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    printf("进程 A 准备就绪，开始写入...\n");

    // 3. 进入临界区（加锁）
    sem_wait(sem); // 相当于 P 操作
    
    // 写入数据
    snprintf(shm_ptr, 1024, "Hello from Process A! Time: 2026");
    sleep(5); // 故意停顿，方便观察互斥效果
    
    // 4. 离开临界区（解锁）
    sem_post(sem); // 相当于 V 操作

    printf("进程 A 写入完毕，解锁。\n");

    // 分离共享内存
    shmdt(shm_ptr);
    // 关闭信号量（注意：并没有销毁它）
    sem_close(sem); 
    return 0;
}
