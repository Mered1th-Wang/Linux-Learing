#include <func.h>

struct text{
	int flag;
	char buf[1024];	
};

int main(){
	int ret;
	int shmid = shmget(1000, sizeof(struct text), 0600|IPC_CREAT);
	struct text *pText = (struct text*)shmat(shmid, NULL, 0);
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
	
	system("clear");
	while(1){
		semop(semid, &sopp, 1);
		if(pText->flag == 1){
			printf("%s\n", pText->buf);
			memset(pText->buf, 0, sizeof(pText->buf));
			pText->flag = 0;
			semop(semid, &sopv, 1);
		}
		else if(pText->flag == 3){
			printf("							%s", pText->buf);
			memset(pText->buf, 0, sizeof(pText->buf));
			pText->flag = 0;
			semop(semid, &sopv, 1);
		}
		else if(pText->flag == 2){
			shmdt(&pText);
			shmctl(shmid, IPC_RMID, NULL); 
			semop(semid, &sopv, 1);
			return 0;
		}
		else{
			semop(semid, &sopv,1);
		}
	}
	return 0;
}
