#include "func.h"

int main(int argc,char **argv)
{
	args_check(argc,3);
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);//IP地址的点分十进制转为网络字节序
	int ret;
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}
	printf("connect success\n");
	int data_len;
	char buf[1000]={0};
	recv_n(sfd,(char*)&data_len,sizeof(int));
	recv_n(sfd,buf,data_len);
	//接文件大小
	off_t file_size,download_size=0;
	recv_n(sfd,(char*)&data_len,sizeof(int));
	recv_n(sfd,(char*)&file_size,data_len);
	int fd=open(buf,O_WRONLY|O_CREAT,0666);
	time_t now,before;
	time(&now);
	before=now;
	while(1)
	{
		ret=recv_n(sfd,(char*)&data_len,sizeof(int));
		if(-1==ret)
		{
			break;
		}
		if(data_len>0)
		{
			ret=recv_n(sfd,buf,data_len);
			if(-1==ret)
			{
				break;
			}
			download_size=download_size+data_len;
			write(fd,buf,data_len);
			time(&now);
			if(now-before>0)
			{
				printf("%5.2f%s\r",(float)download_size*100/file_size,"%");
				fflush(stdout);
				before=now;
			}
		}else{
			printf("%5.2f%s\n",(float)download_size*100/file_size,"%");
			break;
		}
	}
	close(fd);
	close(sfd);
}

