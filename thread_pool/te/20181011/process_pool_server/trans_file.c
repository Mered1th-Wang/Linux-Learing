#include "func.h"

void sigfunc(int signum)
{
	printf("%d is coming\n",signum);
}
int trans_file(int new_fd)
{
	train t;
	signal(SIGPIPE,sigfunc);
	//发送文件名
	t.data_len=strlen(FILENAME);//实际要转换为网络字节序
	strcpy(t.buf,FILENAME);
	int ret;
	ret=send_n(new_fd,(char*)&t,4+t.data_len);
	if(-1==ret)
	{
		goto end;
	}
	//发送文件长度
	t.data_len=sizeof(off_t);
	struct stat buf;
	stat(FILENAME,&buf);
	memcpy(t.buf,&buf.st_size,sizeof(off_t));
	ret=send_n(new_fd,(char*)&t,4+t.data_len);
	if(-1==ret)
	{
		goto end;
	}
	//发送文件内容
	int fd=open(FILENAME,O_RDONLY);
	if(-1==fd)
	{
		perror("open");
		return -1;
	}
	while(t.data_len=read(fd,t.buf,sizeof(t.buf)))
	{
		ret=send_n(new_fd,(char*)&t,4+t.data_len);
		if(-1==ret)
		{
			goto end;
		}
	}
	send_n(new_fd,(char*)&t,4);//发送结束符
end:
	close(new_fd);
}
