#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void msg_handler(int r_fd) {
	char buf[1024];

	ssize_t r_len = read(r_fd, buf, sizeof(buf) - 1);
	while (r_len) {
		if (r_len == -1) {
			perror("read");
			break;
		}
		buf[r_len] = 0;					// 如果读取的内容是字符串，做一个收尾处理
		printf("read size: %zd\n", strlen(buf));
		r_len = read(r_fd, buf, sizeof(buf) - 1);
	}
	printf("remote write closed!\n");
}

int main() {
	int fd = open("abc", O_RDONLY);
	if (fd == -1) {
		perror("open");
		return -1;
	}
	printf("open fifo by reader success!\n");

	msg_handler(fd);

	close(fd);
	return 0;
}