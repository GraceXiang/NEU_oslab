#include <string.h>
#include <time.h>

#include "queue.c"

typedef struct {
    char* name;    // 信号量名称
    int count;     // 空闲资源数
    Queue* queue;  // 进程等待队列，存放阻塞在该信号量的进程PCB
} Semaphore;

char GetCommand();
void display(int Buffer[], Semaphore* BufferEmpty, Semaphore* BufferFull,
             Semaphore* Mutex, Queue* current_process);
bool P(Semaphore* sem, PCB pcb);
bool V(Semaphore* sem);
void Producer(Semaphore* BufferEmpty, Semaphore* Mutex, int Buffer[],
              int* producer_pointer, Queue* current_process);
void Consumer(Semaphore* BufferFull, Semaphore* Mutex, int Buffer[],
              int* consumer_pointer, Queue* current_process);
void ExecutePrimitive(Semaphore* BufferEmpty, Semaphore* BufferFull,
                      Semaphore* Mutex, int Buffer[], Queue* current_process);