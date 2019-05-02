#include <func.h>

struct msgtype{
	long mtype;
	char buf[128];
};

struct text{
       int flag;
       char buf[1024];
};

int msgid;

void SignalHandler(int sigNum){
	int shmid = shmget(1000, sizeof(struct text), 0600|IPC_CREAT);
	struct text *pText = (struct text*)shmat(shmid, NULL, 0);
	int semid = semget(1000, 1, 0600|IPC_CREAT);
	semctl(semid, 0, SETVAL, 1);
	struct sembuf sopp, sopv;
	sopp.sem_num = 0;
	sopp.sem_op = -1;
	sopp.sem_flg = SEM_UNDO;
	sopv.sem_num = 0;
	sopv.sem_op = 1;
	sopv.sem_flg = SEM_UNDO;
	semop(semid, &sopp, 1);
	pText->flag = 2;
	semop(semid, &sopv, 1);

	msgctl(msgid, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID);
	return;
}

int main(int argc, char **argv){
	ARGS_CHECK(argc, 3);
	int ret;
	int fdw = open(argv[1], O_WRONLY);
	ERROR_CHECK(fdw, -1, "open");
	int fdr = open(argv[2], O_RDONLY);
	ERROR_CHECK(fdr, -1, "open");
	system("clear");
	printf("B connect successfully!\n");
	fd_set rdset;

	msgid = msgget(1000, 0600|IPC_CREAT);
	ERROR_CHECK(msgid, -1, "msgget");
	struct msgtype msg;

	int shmid = shmget(1000, sizeof(struct text), 0600|IPC_CREAT);
	struct text *pText = (struct text*)shmat(shmid, NULL, 0);
	int semid = semget(1000, 1, 0600|IPC_CREAT);
	semctl(semid, 0, SETVAL, 1);
	struct sembuf sopp, sopv;
	sopp.sem_num = 0;
	sopp.sem_op = -1;
	sopp.sem_flg = SEM_UNDO;
	sopv.sem_num = 0;
	sopv.sem_op = 1;
	sopv.sem_flg = SEM_UNDO;

	while(1){
		signal(SIGINT,SignalHandler);
		semop(semid, &sopp, 1);
		if(pText->flag == 2){
			msgctl(msgid, IPC_RMID, NULL);
			semctl(semid, 0, IPC_RMID);
			semop(semid, &sopv, 1);
			return 0;
		}
		semop(semid, &sopv, 1);
		FD_ZERO(&rdset);
		FD_SET(STDIN_FILENO, &rdset);
		FD_SET(fdr, &rdset);
		ret = select(fdr + 1, &rdset, NULL, NULL, NULL);
		if(ret > 0){
			if(FD_ISSET(fdr, &rdset)){
				memset(msg.buf, 0, sizeof(msg.buf));
				ret = read(fdr, msg.buf, sizeof(msg.buf));
				if(ret == 0){
					msgctl(msgid, IPC_RMID, NULL);
					semctl(semid, 0, IPC_RMID);
					break;
				}
				msg.mtype = 1;
				ret = msgsnd(msgid, &msg, strlen(msg.buf), 0);
				ERROR_CHECK(ret, -1, "msgsnd");
			}
			if(FD_ISSET(STDIN_FILENO, &rdset)){
				memset(msg.buf, 0, sizeof(msg.buf));
				ret = read(STDIN_FILENO, msg.buf, sizeof(msg.buf));
				if(ret == 0){
					msgctl(msgid, IPC_RMID, NULL);
					semctl(semid, 0, IPC_RMID);
					break;
				}
				write(fdw, msg.buf, strlen(msg.buf) - 1);
				msg.mtype = 1;
				char space[20]="							";
				strcat(space, msg.buf);
				strcpy(msg.buf,space);
				ret = msgsnd(msgid, &msg, strlen(msg.buf) - 1, 0);
				ERROR_CHECK(ret, -1, "msgsnd");
			}
		}
	}
	return 0;
}
