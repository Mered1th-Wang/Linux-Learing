#include <func.h>

struct msgtype{
	long mtype;
	char buf[128];
};

int main(){
	system("clear");
	int ret;
	int msgid = msgget(1000, 0600|IPC_CREAT);
	ERROR_CHECK(msgid, -1, "msgget");
	struct msgtype rcvmsg;
	while(1){
		memset(&rcvmsg, 0, sizeof(struct msgtype));
		ret = msgrcv(msgid, &rcvmsg, sizeof(rcvmsg.buf), 0, 0);
		ERROR_CHECK(ret, -1, "msgrcv");
		rcvmsg.mtype = 0;
		printf("%s\n", rcvmsg.buf);
	}
	return 0;
}
