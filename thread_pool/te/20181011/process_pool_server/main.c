#include "func.h"

int exit_fds[2];
//异步拉起同步
void sigfunc_exit(int sig)
{
	char exit_flag;
	write(exit_fds[1],&exit_flag,1);
}
int main(int argc,char **argv)
{
	args_check(argc,4);
	int process_num=atoi(argv[3]);
	process_data* p=(process_data*)calloc(process_num,sizeof(process_data));
	make_child(p,process_num);//创建子进程
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);//IP地址的点分十进制转为网络字节序
	int ret;
	int reuse=1;
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	pipe(exit_fds);
	//epoll监听sfd及每个子进程的管道对端
	int epfd=epoll_create(1);
	struct epoll_event event,*evs;
	evs=(struct epoll_event*)calloc(process_num+2,sizeof(struct epoll_event));
	event.events=EPOLLIN;
	event.data.fd=sfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	event.data.fd=exit_fds[0];
	epoll_ctl(epfd,EPOLL_CTL_ADD,exit_fds[0],&event);
	int i;
	for(i=0;i<process_num;i++)
	{
		event.data.fd=p[i].fd;
		epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].fd,&event);
	}
	listen(sfd,process_num);
	int ready_fd_num;
	int new_fd,j;
	char child_flag;
	signal(SIGUSR1,sigfunc_exit);
	while(1)
	{
		ready_fd_num=epoll_wait(epfd,evs,process_num+2,-1);
		for(i=0;i<ready_fd_num;i++)
		{
			if(evs[i].data.fd==sfd)
			{
				new_fd=accept(sfd,NULL,NULL);//拿到客户端请求
				for(j=0;j<process_num;j++)
				{
					if(0==p[j].busy)//找到空闲的子进程，把描述符传递给对应子进程
					{
						send_fd(p[j].fd,new_fd,0);
						p[j].busy=1;
						printf("%d is busy\n",p[j].pid);
						break;
					}
				}
				close(new_fd);
			}
			if(evs[i].data.fd==exit_fds[0])
			{
				printf("you can see me\n");
				for(j=0;j<process_num;j++)//通知子进程退出
				{
					send_fd(p[j].fd,0,1);
				}
				for(j=0;j<process_num;j++)
				{
					wait(NULL);
				}
				close(sfd);
				exit(0);
			}
			for(j=0;j<process_num;j++)
			{
				if(evs[i].data.fd==p[j].fd)//收到子进程的通知，把对应子进程标识为非忙碌
				{
					read(p[j].fd,&child_flag,sizeof(char));
					printf("%d is not busy\n",p[j].pid);
					p[j].busy=0;
				}
			}
		}
	}
}
