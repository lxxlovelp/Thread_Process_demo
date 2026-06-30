#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	key_t temp;

	temp = ftok("/tmp",3);
	printf("the temp is %u\n",temp);
}

