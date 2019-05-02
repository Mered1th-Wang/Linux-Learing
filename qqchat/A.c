#include <func.h>

struct text{
	int flag;
	char buf[1024];
};

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
	
	semctl(semid, 0, IPC_RMID);
	exit(0);
}

int main(int argc, char **argv){
	ARGS_CHECK(argc, 3);

	int ret;
	//设置有名管道
	int fdr = open(argv[1], O_RDONLY);
	ERROR_CHECK(fdr, -1, "open");
	int fdw = open(argv[2], O_WRONLY);
	ERROR_CHECK(fdw, -1, "open");
	system("clear");
	printf("A connect successfully!\n");
	char buf[128] = {0};
	fd_set rdset; //定义读文件描述符的集合

	//设置共享内存
	int shmid = shmget(1000, sizeof(struct text), 0600|IPC_CREAT);
	ERROR_CHECK(shmid, -1, "shmget");
	struct text *pText = (struct text*)shmat(shmid, NULL, 0);
	
	//设置锁
	int semid = semget(1000, 1, 0600|IPC_CREAT);
	ERROR_CHECK(semid, -1, "semid");
	ret = semctl(semid, 0, SETVAL, 1);
	ERROR_CHECK(ret, -1, "semctl");
	struct sembuf sopp, sopv;
	sopp.sem_num = 0;
	sopp.sem_op = -1;
	sopp.sem_flg = SEM_UNDO;
	sopv.sem_num = 0;
	sopv.sem_op = 1;
	sopv.sem_flg = SEM_UNDO;
	
	semop(semid, &sopp, 1);
	pText->flag = 0;
	semop(semid, &sopv, 1);
	int i;
	while(1){
		signal(SIGINT, SignalHandler);
		FD_ZERO(&rdset);
		//将STDIN_FILENO和fdr加入集合rdset
		FD_SET(STDIN_FILENO, &rdset);
		FD_SET(fdr, &rdset);
		//检查是否可读
		ret = select(fdr + 1, &rdset, NULL, NULL, NULL);
		if(ret > 0){
			//检查fdr文件描述符是否在rdset集合中
			if(FD_ISSET(fdr, &rdset)){
				memset(buf, 0, sizeof(buf));
				ret = read(fdr, buf, sizeof(buf));
				if(ret == 0){
					printf("byebye!\n");
					semop(semid, &sopp, 1);
					pText->flag = 2;
					semop(semid, &sopv, 1);
					break;
				}
				semop(semid, &sopp, 1);
				pText->flag = 1;
				memset(pText->buf, 0, sizeof(pText->buf));
				for(i = 0;i < strlen(buf);i++){
					pText->buf[i] = buf[i];
				}
				pText->buf[i] = '\0';
				semop(semid, &sopv, 1);
			}
			if(FD_ISSET(STDIN_FILENO, &rdset)){
				memset(buf, 0, sizeof(buf));
				ret = read(STDIN_FILENO, buf, sizeof(buf));
				if(ret == 0){
					printf("byebye!\n");
					break;
				}
				write(fdw, buf, strlen(buf)-1);
				memset(pText->buf, 0, sizeof(pText->buf));
				for(i = 0;i < strlen(buf);i++){
					pText->buf[i] = buf[i];
				}
				pText->buf[i] = '\0';
				semop(semid, &sopp, 1);
				pText->flag = 3;
				semop(semid, &sopv, 1);
			}
		}
	}
	return 0;
}
