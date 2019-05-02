#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"

typedef struct{
    pthread_t *pth_arr; //pthId
    int thread_num; //线程数
    pthread_cond_t cond;//条件变量
    que_t q; //队列
    short start_flag;
}factory_t, *pFactory_t;

#endif
