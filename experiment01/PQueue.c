#include "PQueue.h"

//初始化队列
void InitPQueue(PQueue pQueue) {
    if (NULL == pQueue) return;
    pQueue->next = NULL;
}

//从堆中申请一个节点的内存空间
PNode* InitNode(PCB pcb) {
    PNode* pTmp = (PNode*)malloc(sizeof(PNode));
    pTmp->pcb = pcb;
    pTmp->next = NULL;
    return pTmp;
}

//入队
void Push(PQueue pQueue, PCB pcb) {
    PNode* pTmp = InitNode(pcb);
    PNode* pPre = pQueue;
    PNode* pCur = pQueue->next;
    while (NULL != pCur) {
        if (pCur->pcb.prio > pcb.prio) {
            pTmp->next = pCur;
            pPre->next = pTmp;
            return;
        } else {
            pPre = pCur;
            pCur = pCur->next;
        }
    }
    pPre->next = pTmp;
}

//出队，从队首(front)出
bool Pop(PQueue pQueue, PCB* pPCB) {
    if (!IsEmpty(pQueue)) {
        PNode* pTmp = pQueue->next;
        *pPCB = pTmp->pcb;
        pQueue->next = pTmp->next;
        free(pTmp);
        return true;
    }
    return false;
}

//获取队列长度
int GetPQueueLen(PQueue pQueue) {
    int iCount = 0;
    PNode* pCur = pQueue->next;
    while (NULL != pCur) {
        ++iCount;
        pCur = pCur->next;
    }
    return iCount;
}

//输出队列所有元素
void ShowQueue(PQueue pQueue) {
    PNode* pCur = pQueue->next;
    while (NULL != pCur) {
        printf("\033[40;32m%s(%d) \033[0m", pCur->pcb.name, pCur->pcb.prio);
        pCur = pCur->next;
    }
    printf("\n");
}

void ViewPCB(PQueue pQueue) {
    PNode* pCur = pQueue->next;
    while (NULL != pCur) {
        printf("\033[40;34m%s %d %d %d\033[0m\n", pCur->pcb.name, pCur->pcb.PID,
               pCur->pcb.memory, pCur->pcb.prio);
        pCur = pCur->next;
    }
}

int CalcQueueMemory(PQueue pQueue) {
    int occupiedMemory = 0;
    PNode* pCur = pQueue->next;
    while (NULL != pCur) {
        occupiedMemory += pCur->pcb.memory;
        pCur = pCur->next;
    }
    return occupiedMemory;
}

//队列为空则返回true
bool IsEmpty(PQueue pQueue) { return pQueue->next == NULL; }

//获取队首元素
bool GetFront(PQueue pQueue, PCB* pPCB) {
    if (!IsEmpty(pQueue)) {
        *pPCB = pQueue->next->pcb;
        return true;
    }
    return false;
}

//销毁队列(释放所有节点的内存空间)
void Destroy(PQueue pQueue) {
    PNode* pCur = pQueue->next;
    while (NULL != pCur) {
        pQueue = pCur->next;
        free(pCur);
        pCur = pQueue;
    }
}