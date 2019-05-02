#include "func.h"

int send_fd(int pipefd,int fd)
{
	struct msghdr msg;
	struct iovec iov[2];
	char buf1[10]="hello";
	char buf2[10]="world";
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;//变长结构体
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=sendmsg(pipefd,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return -1;
	}
	return 0;
}
int recv_fd(int pipefd,int* fd)
{
	struct msghdr msg;
	struct iovec iov[2];
	char buf1[10]="hello";
	char buf2[10]="world";
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;//变长结构体
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=recvmsg(pipefd,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return -1;
	}
	*fd=*(int*)CMSG_DATA(cmsg);
	return 0;
}


int main()
{
	int fds[2];
	socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
	if(!fork())
	{
		close(fds[1]);
		int fd;
		recv_fd(fds[0],&fd);
		char buf[128]={0};
		printf("child fd=%d,fds[0]=%d\n",fd,fds[0]);
		read(fd,buf,sizeof(buf));
		printf("buf=%s\n",buf);
	}else{
		close(fds[0]);
		int fd=open("file",O_RDWR);
		printf("parent fd=%d\n",fd);
		send_fd(fds[1],fd);
		wait(NULL);
	}
}
