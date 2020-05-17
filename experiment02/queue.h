#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR -99  // 错误标志

typedef struct process_control_block {
    char name[15];  // 进程名字
    int PID;        // 进程号
    int pointer;    // Buffer共享缓冲区指针
    int data;       // Buffer共享缓冲区数据
} PCB;

typedef struct Node {
    PCB pcb;
    struct Node* next;
} QNode;

typedef struct {
    QNode* front;  // 指向对头节点
    QNode* rear;   // 指向队尾节点
} Queue;

Queue* CreateQueue();
bool IsEmptyQ(Queue* q);
void Enqueue(Queue* q, PCB item);
int Dequeue(Queue* q, PCB* item);
void PrintQueue(Queue* q);
