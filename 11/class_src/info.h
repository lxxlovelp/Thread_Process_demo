#ifndef INFO_H
#define INFO_H
#include <unistd.h>

#define SHMEM_SIZE	(0x1<<10)
#define KEY_PATHNAME	"/tmp"
#define KEY_INT			10

typedef struct {
	pid_t pid;
	char text[1];
} msg_t;

#endif