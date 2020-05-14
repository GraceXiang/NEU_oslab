#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "PQueue.c"

#define MEMORY_SIZE 100  // 内存大小100M

int GetOption();
bool CalcMemorySpace(PCB pcb, PNode *Running, PNode *Ready, PNode *Blocked);
void PrintQueue(PNode New, PNode Running, PNode Ready, PNode Blocked,
                PNode ReadySuspend, PNode BlockedSuspend);
bool Dispatch(PNode *Ready, PNode *Running, PNode *ReadySuspend,
              PNode *BlockedSuspend, PNode *Blocked);
bool Timeout(PNode *Running, PNode *Ready, PNode *ReadySuspend,
             PNode *BlockedSuspend, PNode *Blocked);
bool EventWait(PNode *Running, PNode *Blocked, PNode *Ready,
               PNode *ReadySuspend, PNode *BlockedSuspend);
bool EventOccurs(PNode *Blocked, PNode *Ready, PNode *Running,
                 PNode *ReadySuspend, PNode *BlockedSuspend);
void Admit(PNode *New, PNode *Ready);
void Create(PNode *New, PNode *Running, PNode *Ready, PNode *Blocked,
            PNode *ReadySuspend, PNode *BlockedSuspend);
bool Release(PNode *Running, PNode *New, PNode *Ready, PNode *Blocked,
             PNode *ReadySuspend, PNode *BlockedSuspend);
void Suspend(PNode *Blocked, PNode *BlockedSuspend);
void Active(PNode *ReadySuspend, PNode *BlockedSuspend, PNode *Ready,
            PNode *Running, PNode *Blocked);
bool OutsideMemoryEventOccur(PNode *BlockedSuspend, PNode *ReadySuspend);
