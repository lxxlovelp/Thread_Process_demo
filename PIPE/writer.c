#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void chat_handler(int w_fd) {
	char buf[1024];
	printf("input content: ");
	fflush(stdout);
	char *s = fgets(buf, sizeof(buf), stdin);
	while (s) {
		buf[strlen(buf) - 1] = 0;
		ssize_t w_len = write(w_fd, buf, strlen(buf));
		if (w_len == -1) {
			perror("write");
			break;
		}
		printf("input content: ");
		fflush(stdout);
		s = fgets(buf, sizeof(buf), stdin);
	}
}

int main() {
	int fd = open("abc", O_WRONLY);
	if (fd == -1) {
		perror("open");
		return -1;
	}
	printf("open fifo by writer success!\n");

	chat_handler(fd);

	close(fd);
	return 0;
}
