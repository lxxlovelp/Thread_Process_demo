#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUF_SIZE	256
#define PROJ_ID		32
#define PATH_NAME	"."

struct mymsgbuf{
	long msgtype;
	char ctrlstring[BUF_SIZE];
}; 

int main(void)
{
	/*用户自定义消息缓冲*/
	struct mymsgbuf msgbuffer;
	int		qid;    /*消息队列标识符*/
	int		msglen;
	key_t	msgkey;

	/*获取键值*/
	if((msgkey = ftok (PATH_NAME, PROJ_ID)) == -1)
	{
		perror ("ftok error!\n");
		exit (1);
	}

	printf("the msgkey is %x\n",msgkey);
	/*创建消息队列*/
	if((qid = msgget (msgkey, IPC_CREAT|0660)) == -1)
	{
		perror ("msgget error!\n");
		exit (1);
	}
	/*填充消息结构，发送到消息队列*/
	msgbuffer.msgtype = 3;
	strcpy (msgbuffer.ctrlstring , "Hello,message queue!!!");
	msglen = sizeof(msgbuffer) - 4;
	if(msgsnd (qid, &msgbuffer, msglen, 0) == -1)
	{
		perror ("msgget error!\n");
		exit (1);
	}

	memset(&msgbuffer,0,sizeof(msgbuffer));
	if (msgrcv(qid, &msgbuffer, msglen, 100, 0) == -1)  /*读取数据*/
	{
		perror ("msgrcv error!\n");
		exit (1);
	}

	printf("the recv is %s\n",msgbuffer.ctrlstring);

	exit(0);
}

