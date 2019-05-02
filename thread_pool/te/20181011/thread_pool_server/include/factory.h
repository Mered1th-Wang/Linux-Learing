#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"

typedef struct{
	pthread_t *pth_arr;
	int thread_num;
	pthread_cond_t cond;
	que_t que;
	short start_flag;
}factory_t,*pfactory_t;
#endif
