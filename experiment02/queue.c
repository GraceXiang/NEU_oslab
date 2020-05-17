#include "queue.h"

Queue* CreateQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        printf("No enough space!\n");
        return NULL;
    }
    q->front = NULL;
    q->rear = NULL;
    return q;
}

bool IsEmptyQ(Queue* q) { return (q->front == NULL); }

void Enqueue(Queue* q, PCB item) {  // 将进程 pcb 插入队尾
    QNode* qNode = (QNode*)malloc(sizeof(QNode));
    if (!qNode) {
        printf("No enough space!\n");
        return;
    }
    qNode->pcb = item;
    qNode->next = NULL;
    if (q->front == NULL) {
        q->front = qNode;
    }
    if (q->rear == NULL) {
        q->rear = qNode;
    } else {
        q->rear->next = qNode;
        q->rear = qNode;
    }
}

int Dequeue(Queue* q, PCB* item) {  // 删除队首元素
    if (IsEmptyQ(q)) {
        // printf("The queue is empty\n");
        return ERROR;
    }
    QNode* temp = q->front;
    if (q->front == q->rear) {  // 若队列只有一个元素
        q->front = NULL;
        q->rear = NULL;
    } else {
        q->front = q->front->next;
    }
    *item = temp->pcb;
    free(temp);
    return 0;
}

void PrintQueue(Queue* q) {
    if (IsEmptyQ(q)) {
        printf("The queue is empty!\n");
        return;
    }
    // printf("Queue data element as follows:\n");
    QNode* qNode = q->front;
    while (qNode != NULL) {
        printf("\033[40;32m%s \033[0m", qNode->pcb.name);
        qNode = qNode->next;
    }
    printf("\n");
}
