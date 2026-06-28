/* 统计用户输入的字符个数 
 * 1. 输入任务
 * 2. 统计任务
 * 3. 两个任务的数据通信：定义一个全局变量，输入任务写入这个对象，统计任务也从这个对象中获取
 * 临界资源，同步、互斥，同步条件，输入有内容，才统计
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// 1. 定义任务间的通信对象结构
typedef struct {
	char buf[1024];		// 任务间的存放的缓存区，临界资源
	volatile int flags;			// 用户自定义的条件行为	1：输入完成  0：处理完数据，等待新的采集
	pthread_mutex_t mutex;		// 实现互斥
	pthread_cond_t cond;		// 实现自定义的同步效果
	pthread_t handler_id;
} MSG_t;

void init_msg(MSG_t *msg) {
	memset(msg->buf, 0, sizeof(msg->buf));
	msg->flags = 0;
	pthread_mutex_init(&msg->mutex, NULL);
	pthread_cond_init(&msg->cond, NULL);
}
MSG_t user_msg;

void input_task(void) {
	while (1) {
		pthread_mutex_lock(&user_msg.mutex);
		// 等待新的采集状态
		while (user_msg.flags != 0) {
			pthread_cond_wait(&user_msg.cond, &user_msg.mutex);	
		}
		// 采集数据
		printf("please input content: ");
		fflush(stdout);
		fgets(user_msg.buf, sizeof(user_msg.buf), stdin);
		// 回车去掉 123\n
		user_msg.buf[strlen(user_msg.buf) - 1] = 0;
		// 如果用户输入的是quit，那么该任务就该退出，统计任务也要退出
		if (strncmp(user_msg.buf, "quit", 4) == 0) {
			printf("ready quit...\n");
			
			pthread_mutex_unlock(&user_msg.mutex);
			pthread_cancel(user_msg.handler_id);
			pthread_exit(NULL);
		}

		user_msg.flags = 1;
		pthread_cond_signal(&user_msg.cond);
		pthread_mutex_unlock(&user_msg.mutex);
	}
}

void count_task(void) {
	while (1) {
		pthread_mutex_lock(&user_msg.mutex);
		// 等待输入完成事件的发生
		while (user_msg.flags != 1) {			
			pthread_cond_wait(&user_msg.cond, &user_msg.mutex);		// 释放锁、加锁
		}
		printf("input num: %zd\n", strlen(user_msg.buf));		// 处理事件
		sleep(1);
		// 通知输入事件可以继续采集数据
		user_msg.flags = 0;
		pthread_cond_signal(&user_msg.cond);
		pthread_mutex_unlock(&user_msg.mutex);
	}
}

int main() {
	pthread_t thread1, thread2;
	init_msg(&user_msg);

	if (pthread_create(&thread1, NULL,
					   (void *(*)(void *))input_task, NULL)) {
		perror("create thread1");
		return -1;
	}
	if (pthread_create(&thread2, NULL,
					   (void *(*)(void *))count_task, NULL)) {
		perror("create thread1");
		return -1;
	}
	user_msg.handler_id = thread2;

	pthread_join(thread1, NULL);
	printf("=========================\n");
	pthread_join(thread2, NULL);
	return 0;
}
