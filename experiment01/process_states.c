#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "PQueue.c"

#define MEMORY_SIZE 100  // 内存大小100M

int GetOption() {
    int option;
    printf("------------------\n");
    printf("[1].Dispatch\n");
    printf("[2].Timeout\n");
    printf("[3].Event Wait\n");
    printf("[4].Event Occurs\n");
    printf("[5].Create\n");
    printf("[6].Release\n");
    printf("[7].View PCB\n");
    printf("[8].View Queue\n");
    printf("[0].Exit the demo program\n");
    printf("Enter the command number to select: ");

    while (!scanf("%d", &option)) {  // 过滤非数字输入
        while (getchar() != '\n') {
        }
        printf("No such number, please check and enter again!");
    }

    return option;
}

bool CalcMemorySpace(PCB pcb, PNode *Running, PNode *Ready, PNode *Blocked) {
    return (pcb.memory + CalcQueueMemory(Running) + CalcQueueMemory(Ready) +
            CalcQueueMemory(Blocked)) < MEMORY_SIZE;
}

void PrintQueue(PNode New, PNode Running, PNode Ready, PNode Blocked) {
    printf("\n");
    printf("\033[40;32mNew queue: \033[0m");
    ShowQueue(&New);
    printf("\033[40;32mRunning queue: \033[0m");
    ShowQueue(&Running);
    printf("\033[40;32mReady queue: \033[0m");
    ShowQueue(&Ready);
    printf("\033[40;32mBlocked queue: \033[0m");
    ShowQueue(&Blocked);
}

bool Dispatch(PNode *Ready, PNode *Running) {
    if (IsEmpty(Ready)) {
        printf("\033[40;31mDispatch error: Ready queue is empty!\033[0m\n");
        return false;
    }
    if (!IsEmpty(Running)) {
        printf("\033[40;31mDispatch error: a program is running!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(Ready, &pcb);
    Push(Running, pcb);
    return true;
}

bool Timeout(PNode *Running, PNode *Ready) {
    if (IsEmpty(Running)) {
        printf("\033[40;31mWrong operation: no running program!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(Running, &pcb);
    Push(Ready, pcb);
    Dispatch(Ready, Running);
    return true;
}

bool EventWait(PNode *Running, PNode *Blocked, PNode *Ready) {
    if (IsEmpty(Running)) {
        printf("\033[40;31mWrong operation: no running program!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(Running, &pcb);
    Push(Blocked, pcb);
    if (!IsEmpty(Ready)) {
        Dispatch(Ready, Running);
    }
    return true;
}

bool EventOccurs(PNode *Blocked, PNode *Ready, PNode *Running) {
    if (IsEmpty(Blocked)) {
        printf("\033[40;31mWrong operation: Blocked queue is empty!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(Blocked, &pcb);
    if (IsEmpty(Running) && IsEmpty(Ready)) {
        Push(Ready, pcb);
        Dispatch(Ready, Running);
    } else {
        Push(Ready, pcb);
    }
    return true;
}

void Admit(PNode *New, PNode *Ready) {
    if (IsEmpty(New)) {
        return;
    }
    PCB pcb;
    Pop(New, &pcb);
    Push(Ready, pcb);
}

void Create(PNode *New, PNode *Running, PNode *Ready, PNode *Blocked) {
    PCB pcb;
    printf("\033[40;36mPlease enter the process name: \033[0m");
    scanf("%s", pcb.name);
    getchar();
    printf("\033[40;36mPlease enter the process memory: \033[0m");
    scanf("%d", &pcb.memory);
    getchar();
    printf("\033[40;36mPlease enter the process prio: \033[0m");
    scanf("%d", &pcb.prio);
    getchar();
    Push(New, pcb);
    if (CalcMemorySpace(pcb, Running, Ready, Blocked)) {
        Admit(New, Ready);
    }
    PrintQueue(*New, *Running, *Ready, *Blocked);
}

bool Release(PNode *Running, PNode *New, PNode *Ready, PNode *Blocked) {
    if (IsEmpty(Running)) {
        printf("\033[40;31mRelease error: Running queue is empty!\033[0m\n");
        return false;
    }
    PCB tPcb, pcb;
    Pop(Running, &tPcb);
    GetFront(New, &pcb);
    while (CalcMemorySpace(pcb, Running, Ready, Blocked)) {
        Admit(New, Ready);
        if (!GetFront(New, &pcb)) {
            break;
        }
    }
    if (!IsEmpty(Ready)) {
        Dispatch(Ready, Running);
    }
    return true;
}

int main(int argc, const char *argv[]) {
    int option;
    PNode New;
    InitPQueue(&New);  // 创建态队列
    PNode Running;
    InitPQueue(&Running);  // 执行态队列
    PNode Ready;
    InitPQueue(&Ready);  // 就绪态队列
    PNode Blocked;
    InitPQueue(&Blocked);  // 阻塞态队列

    PCB pcb_A = {"A", 1010, 20, 3};
    PCB pcb_B = {"B", 1015, 30, 2};
    PCB pcb_C = {"C", 1045, 10, 4};
    PCB pcb_D = {"D", 1000, 15, 1};
    PCB pcb_E = {"E", 1020, 15, 3};

    Push(&Running, pcb_A);
    Push(&Ready, pcb_B);
    Push(&Ready, pcb_C);
    Push(&Blocked, pcb_D);
    Push(&Blocked, pcb_E);

    PrintQueue(New, Running, Ready, Blocked);

    option = GetOption();
    while (option != 0) {
        switch (option) {
            case 1:
                if (Dispatch(&Ready, &Running)) {
                    PrintQueue(New, Running, Ready, Blocked);
                }
                option = GetOption();
                break;
            case 2:
                if (Timeout(&Running, &Ready)) {
                    PrintQueue(New, Running, Ready, Blocked);
                }
                option = GetOption();
                break;
            case 3:
                if (EventWait(&Running, &Blocked, &Ready)) {
                    PrintQueue(New, Running, Ready, Blocked);
                }
                option = GetOption();
                break;
            case 4:
                if (EventOccurs(&Blocked, &Ready, &Running)) {
                    PrintQueue(New, Running, Ready, Blocked);
                }
                option = GetOption();
                break;
            case 5:
                Create(&New, &Running, &Ready, &Blocked);
                option = GetOption();
                break;
            case 6:
                if (Release(&Running, &New, &Ready, &Blocked)) {
                    PrintQueue(New, Running, Ready, Blocked);
                }
                option = GetOption();
                break;
            case 7:
                printf("\033[40;34mName - PID - Memory - prio:\033[0m\n");
                ViewPCB(&New);
                ViewPCB(&Running);
                ViewPCB(&Ready);
                ViewPCB(&Blocked);
                option = GetOption();
                break;
            case 8:
                PrintQueue(New, Running, Ready, Blocked);
                option = GetOption();
                break;
            default:
                printf("No such number, please check and enter again!\n");
                option = GetOption();
                break;
        }
    }
    return 0;
}