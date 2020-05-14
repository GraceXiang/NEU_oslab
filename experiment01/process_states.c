#include "process_states.h"

int main(int argc, const char *argv[]) {
    int option, status;
    PNode New;
    InitPQueue(&New);
    // 创建态队列(创建新进程时，内存空间足够则加入到就绪队列，否则加入就绪/挂起队列)
    PNode Running;
    InitPQueue(&Running);  // 执行态队列
    PNode Ready;
    InitPQueue(&Ready);  // 就绪态队列
    PNode Blocked;
    InitPQueue(&Blocked);  // 阻塞态队列
    PNode ReadySuspend;
    InitPQueue(&ReadySuspend);  // 就绪/挂起队列
    PNode BlockedSuspend;
    InitPQueue(&BlockedSuspend);  // 阻塞/挂起队列

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

    PrintQueue(New, Running, Ready, Blocked, ReadySuspend, BlockedSuspend);

    option = GetOption();
    while (option != 0) {
        switch (option) {
            case 1:
                if (Dispatch(&Ready, &Running, &ReadySuspend, &BlockedSuspend,
                             &Blocked)) {
                    printf("Info: Dispatch success:");
                    PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                               BlockedSuspend);
                }
                option = GetOption();
                break;
            case 2:
                if (Timeout(&Running, &Ready, &ReadySuspend, &BlockedSuspend,
                            &Blocked)) {
                    printf("Info: Timeout success:");
                    PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                               BlockedSuspend);
                }
                option = GetOption();
                break;
            case 3:
                if (EventWait(&Running, &Blocked, &Ready, &ReadySuspend,
                              &BlockedSuspend)) {
                    printf("Info: Event Wait success:");
                    PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                               BlockedSuspend);
                }
                option = GetOption();
                break;
            case 4:
                if (EventOccurs(&Blocked, &Ready, &Running, &ReadySuspend,
                                &BlockedSuspend)) {
                    printf("Info: Event Occur success:");
                    PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                               BlockedSuspend);
                }
                option = GetOption();
                break;
            case 5:
                Create(&New, &Running, &Ready, &Blocked, &ReadySuspend,
                       &BlockedSuspend);
                printf("Info: Create a new process successful:");
                PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                           BlockedSuspend);
                option = GetOption();
                break;
            case 6:
                if (Release(&Running, &New, &Ready, &Blocked, &ReadySuspend,
                            &BlockedSuspend)) {
                    printf("Info: Release success:");
                    PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                               BlockedSuspend);
                }
                option = GetOption();
                break;
            case 7:
                if (OutsideMemoryEventOccur(&BlockedSuspend, &ReadySuspend)) {
                    printf("Info: Outside Memory Event Occur success:");
                    PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                               BlockedSuspend);
                    printf("Info: Try to touch off Active:");
                    Active(&ReadySuspend, &BlockedSuspend, &Ready, &Running,
                           &Blocked);
                    PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                               BlockedSuspend);
                }
                option = GetOption();
                break;
            case 8:
                printf("\033[40;34mName - PID - Memory - prio:\033[0m\n");
                ViewPCB(&New);
                ViewPCB(&Running);
                ViewPCB(&Ready);
                ViewPCB(&Blocked);
                ViewPCB(&ReadySuspend);
                ViewPCB(&BlockedSuspend);
                option = GetOption();
                break;
            case 9:
                PrintQueue(New, Running, Ready, Blocked, ReadySuspend,
                           BlockedSuspend);
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

int GetOption() {
    int option;
    printf("------------------\n");
    printf("[1].Dispatch\n");
    printf("[2].Timeout\n");
    printf("[3].Event Wait\n");
    printf("[4].Event Occurs\n");
    printf("[5].Create\n");
    printf("[6].Release\n");
    printf("[7].Outside Memory Event Occur\n");
    printf("[8].View PCB\n");
    printf("[9].View Queue\n");
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

void PrintQueue(PNode New, PNode Running, PNode Ready, PNode Blocked,
                PNode ReadySuspend, PNode BlockedSuspend) {
    printf("\n");
    printf("\033[40;32mNew queue: \033[0m");
    ShowQueue(&New);
    printf("\033[40;32mRunning queue: \033[0m");
    ShowQueue(&Running);
    printf("\033[40;32mReady queue: \033[0m");
    ShowQueue(&Ready);
    printf("\033[40;32mBlocked queue: \033[0m");
    ShowQueue(&Blocked);
    printf("\033[40;32mReadySuspend queue: \033[0m");
    ShowQueue(&ReadySuspend);
    printf("\033[40;32mBlockedSuspend queue: \033[0m");
    ShowQueue(&BlockedSuspend);
}

bool Dispatch(PNode *Ready, PNode *Running, PNode *ReadySuspend,
              PNode *BlockedSuspend, PNode *Blocked) {
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
    if (IsEmpty(Ready)) {
        Active(ReadySuspend, BlockedSuspend, Ready, Running, Blocked);
    }
    return true;
}

bool Timeout(PNode *Running, PNode *Ready, PNode *ReadySuspend,
             PNode *BlockedSuspend, PNode *Blocked) {
    if (IsEmpty(Running)) {
        printf("\033[40;31mWrong operation: no running program!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(Running, &pcb);
    Push(Ready, pcb);
    Dispatch(Ready, Running, ReadySuspend, BlockedSuspend, Blocked);
    return true;
}

bool EventWait(PNode *Running, PNode *Blocked, PNode *Ready,
               PNode *ReadySuspend, PNode *BlockedSuspend) {
    if (IsEmpty(Running)) {
        printf("\033[40;31mWrong operation: no running program!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(Running, &pcb);
    Push(Blocked, pcb);
    if (!IsEmpty(Ready)) {
        Active(ReadySuspend, BlockedSuspend, Ready, Running, Blocked);
    }
    Dispatch(Ready, Running, ReadySuspend, BlockedSuspend, Blocked);
    return true;
}

bool EventOccurs(PNode *Blocked, PNode *Ready, PNode *Running,
                 PNode *ReadySuspend, PNode *BlockedSuspend) {
    if (IsEmpty(Blocked)) {
        printf("\033[40;31mWrong operation: Blocked queue is empty!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(Blocked, &pcb);
    if (IsEmpty(Running) && IsEmpty(Ready)) {
        Push(Ready, pcb);
        Dispatch(Ready, Running, ReadySuspend, BlockedSuspend, Blocked);
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

void Create(PNode *New, PNode *Running, PNode *Ready, PNode *Blocked,
            PNode *ReadySuspend, PNode *BlockedSuspend) {
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

    if (CalcMemorySpace(pcb, Running, Ready, Blocked)) {  // 加入到就绪队列
        Push(New, pcb);
        Admit(New, Ready);
        // PrintQueue(*New, *Running, *Ready, *Blocked);
    } else {
        Push(ReadySuspend, pcb);  // 加入就绪/挂起队列
        // Active(ReadySuspend, BlockedSuspend, Ready, Running, Blocked);
    }
}

bool Release(PNode *Running, PNode *New, PNode *Ready, PNode *Blocked,
             PNode *ReadySuspend, PNode *BlockedSuspend) {
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
        Dispatch(Ready, Running, ReadySuspend, BlockedSuspend, Blocked);
    }
    return true;
}

void Suspend(PNode *Blocked, PNode *BlockedSuspend) {
    if (!IsEmpty(Blocked)) {
        PCB pcb;
        Pop(Blocked, &pcb);
        Push(BlockedSuspend, pcb);
    }
}

void Active(PNode *ReadySuspend, PNode *BlockedSuspend, PNode *Ready,
            PNode *Running, PNode *Blocked) {
    PCB pcb, pcb_ready;
    if (IsEmpty(ReadySuspend)) {
        return;
    }
    GetFront(ReadySuspend, &pcb);
    do {  // 至少进行一次 阻塞->阻塞/挂起 交换
        Suspend(Blocked, BlockedSuspend);
    } while (!CalcMemorySpace(pcb, Running, Ready, Blocked) &&
             !IsEmpty(Blocked));

    if (IsEmpty(Ready)) {  // Ready队列为空，直接交换
        Pop(ReadySuspend, &pcb);
        Push(Ready, pcb);
        if (IsEmpty(Running)) {
            Dispatch(Ready, Running, ReadySuspend, BlockedSuspend, Blocked);
        }
    } else {  // Reay队列不为空，但处于ReadySuspend队列的进程比处于Reay队列的任何进程的优先级都高时，也会交换
        GetFront(Ready, &pcb_ready);
        if (pcb.prio > pcb_ready.prio) {
            Pop(ReadySuspend, &pcb);
            Push(Ready, pcb);
            if (IsEmpty(Running)) {
                Dispatch(Ready, Running, ReadySuspend, BlockedSuspend, Blocked);
            }
        }
    }
}

bool OutsideMemoryEventOccur(PNode *BlockedSuspend, PNode *ReadySuspend) {
    if (IsEmpty(BlockedSuspend)) {
        printf(
            "\033[40;31mWrong operation: BlockedSuspend queue is "
            "empty!\033[0m\n");
        return false;
    }
    PCB pcb;
    Pop(BlockedSuspend, &pcb);
    Push(ReadySuspend, pcb);
    return true;
}
