#ifndef __WORK_QUE__
#define __WORK_QUE__
#include "head.h"

typedef struct node{
    int new_fd;
    struct node *pNext;
}Node, *pNode; 

typedef struct tag_node{
    int nd_sockfd;
    struct tag_node* nd_next;
}node_t, *pNode_t;//元素结构体，存储实际client fd

typedef struct tag_que{
    pNode_t que_head, que_tail;
    int que_capacity;
    int que_size;
    pthread_mutex_t que_mutex;
}que_t, *pque_t; //描述队列的结构体

void que_init(pque_t, int);
void que_insert(pque_t, pNode_t);
void que_get(pque_t, pNode_t);

#endif
