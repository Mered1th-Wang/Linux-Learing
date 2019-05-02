#include "func.h"

void make_child(process_data *p,int process_num)
{
	int fds[2];
	int i;
	pid_t pid;
	int ret;
	for(i=0;i<process_num;i++)
	{
		ret=socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
		if(-1==ret)
		{
			return;
		}
		pid=fork();
		if(0==pid)
		{
			close(fds[1]);
			child_handle(fds[0]);
		}
		close(fds[0]);
		p[i].pid=pid;//初始化子进程pid
		p[i].fd=fds[1];//初始化管道
	}
}
void child_handle(int fd)
{
	int new_fd;//接收父进程给的任务
	int exit_flag=0;
	while(1)
	{
		recv_fd(fd,&new_fd,&exit_flag);//接收任务
		if(exit_flag)
		{
			printf("exit ok\n");
			close(fd);
			exit(0);
		}
		//给客户端发送文件
		trans_file(new_fd);
		printf("file send success\n");
		close(new_fd);
		write(fd,&new_fd,1);//通知父进程完成任务
	}
}
