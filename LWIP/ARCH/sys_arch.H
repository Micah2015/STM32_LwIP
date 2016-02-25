
typedef unsigned char OS_EVENT;

#define MAX_QUEUES         10

typedef struct {	 /*  LwIP邮箱数据结构  */
    OS_EVENT*   pQ_Mbox;
    void*       pvQ_msgQueue[MAX_QUEUES];
} LwIP_Q_Mbox, *PLwIP_Q_Mbox;

typedef u8_t             sys_thread_t;
typedef PLwIP_Q_Mbox     sys_mbox_t;
typedef OS_EVENT *       sys_sem_t;

#ifndef sys_mutex_set_invalid
//#define sys_mutex_set_invalid
typedef unsigned char   sys_mutex_t;
#endif
