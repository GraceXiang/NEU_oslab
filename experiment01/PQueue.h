#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct process_control_block {
    char name[15];  // 进程名字
    int PID;        // 进程号
    int memory;     // 内存
    int prio;       // 进程优先级，值越小，优先级越高
} PCB;

typedef struct _PNode {
    PCB pcb;
    struct _PNode* next;
} PNode, *PQueue;

void InitPQueue(PQueue pQueue);     //初始化队列
PNode* InitNode(PCB pcb);           //从堆中申请一个节点的内存空间
void Push(PQueue pQueue, PCB pcb);  //入队，从队尾(rear)入
bool Pop(PQueue pQueue, PCB* pPCB);  //出队，从队首(front)出
int GetPQueueLen(PQueue pQueue);     //获取队列长度
void ShowQueue(PQueue pQueue);       //输出队列所有元素
void ViewPCB(PQueue pQueue);
int CalcQueueMemory(PQueue pQueue);
bool IsEmpty(PQueue pQueue);              //队列为空则返回true
bool GetFront(PQueue pQueue, PCB* pPCB);  //获取队首元素
void Destroy(PQueue pQueue);  //销毁队列(释放所有节点的内存空间)