#include "producer_consumer.h"

int producer_count = 0;
int consumer_count = 0;
int count = 6;

int main(int argc, const char* argv[]) {
    int Buffer[] = {1, 2, 3, 4, 5, 0, 0, 0, 0, 0};  // 共享缓冲区，数据为0表示空
    int producer_pointer = 4;                       // 生产者指针
    int consumer_pointer = -1;                      // 消费者指针
    Semaphore* BufferEmpty = (Semaphore*)malloc(sizeof(Semaphore));
    Semaphore* BufferFull = (Semaphore*)malloc(sizeof(Semaphore));
    Semaphore* Mutex = (Semaphore*)malloc(sizeof(Semaphore));

    // 初始化信号量
    BufferEmpty->name = "BufferEmpty";
    BufferEmpty->count = 5;
    BufferEmpty->queue = CreateQueue();
    BufferFull->name = "BufferFull";
    BufferFull->count = 5;
    BufferFull->queue = CreateQueue();
    Mutex->name = "Mutex";
    Mutex->count = 1;
    Mutex->queue = CreateQueue();

    Queue* current_process = CreateQueue();  // 存放当前正在运行的进程

    char command = GetCommand();
    while (command != 'q') {
        switch (command) {
            case 'p':
                // 模拟生产者进程(执行p原语)
                Producer(BufferEmpty, Mutex, Buffer, &producer_pointer,
                         current_process);
                display(Buffer, BufferEmpty, BufferFull, Mutex,
                        current_process);
                command = GetCommand();
                break;
            case 'c':
                // 模拟消费者进程(执行p原语)
                Consumer(BufferFull, Mutex, Buffer, &consumer_pointer,
                         current_process);
                display(Buffer, BufferEmpty, BufferFull, Mutex,
                        current_process);
                command = GetCommand();
                break;
            case 'e':
                // 执行 v 原语
                ExecutePrimitive(BufferEmpty, BufferFull, Mutex, Buffer,
                                 current_process);
                display(Buffer, BufferEmpty, BufferFull, Mutex,
                        current_process);
                command = GetCommand();
                break;
            default:
                printf("No such operation, please enter again:\n");
                command = GetCommand();
                break;
        }
    }
}

char GetCommand() {
    char command;
    printf("\033[;36mEnter 'p' to simulate a producer process.\033[0m\n");
    printf("\033[;36mEnter 'c' to simulate a consumer process.\033[0m\n");
    printf("\033[;36mEnter 'e' to Execute v primitive.\033[0m\n");
    scanf("%c", &command);
    while (getchar() != '\n') {
    }
    fflush(stdin);
    return command;
}

void display(int Buffer[], Semaphore* BufferEmpty, Semaphore* BufferFull,
             Semaphore* Mutex, Queue* current_process) {
    int i;
    printf("--------------------------------------------------\n");
    printf("|");
    for (i = 0; i < 10; i++) {
        printf(" %2d |", Buffer[i]);
    }
    printf("\n--------------------------------------------------\n");
    printf(
        "\033[40;32mBufferEmpty count: %d   BufferFull count: %d    Mutex "
        "count: %d\033[0m\n",
        BufferEmpty->count, BufferFull->count, Mutex->count);
    if (!IsEmptyQ(current_process)) {
        printf("\033[40;32mcurrent process name: %s PID: %d\033[0m\n",
               current_process->front->pcb.name,
               current_process->front->pcb.PID);
    }
    if (!IsEmptyQ(BufferEmpty->queue)) {
        printf("\033[40;32mBufferEmpty queue: \033[0m");
        PrintQueue(BufferEmpty->queue);
    }
    if (!IsEmptyQ(BufferFull->queue)) {
        printf("\033[40;32mBufferFull queue: \033[0m");
        PrintQueue(BufferFull->queue);
    }
    if (!IsEmptyQ(Mutex->queue)) {
        printf("\033[40;32mMutex queue: \033[0m");
        PrintQueue(Mutex->queue);
    }
}

/*
申请资源成功返回 true
申请资源失败(加入该信号量的阻塞队列)返回 false
*/
bool P(Semaphore* sem, PCB pcb) {
    sem->count--;
    if (sem->count < 0) {
        printf(
            "\033[40;37mAttention: Progress %s joins the %s blocking "
            "queue!\033[0m\n",
            pcb.name, sem->name);
        Enqueue(sem->queue, pcb);
        return false;
    }
    return true;
}

/*
有进程处于阻塞状态返回 true
没有则返回 false
*/
bool V(Semaphore* sem) {
    sem->count++;
    if (sem->count <= 0) {
        // PCB pcb;
        // Dequeue(sem->queue, &pcb);
        return true;
    }
    return false;
}

void Producer(Semaphore* BufferEmpty, Semaphore* Mutex, int Buffer[],
              int* producer_pointer, Queue* current_process) {
    PCB pcb = {"p", 0, -1, -1};
    char buf[15];

    producer_count++;
    sprintf(buf, "%d", producer_count);

    strcat(pcb.name, buf);

    srand((unsigned)time(NULL));
    pcb.PID = rand() % 1005 + 1000;  // PID范围为 1000-2004

    pcb.pointer = (++(*producer_pointer)) % 10;
    pcb.data = count++;

    if (!P(BufferEmpty, pcb)) {
        return;
    }
    if (!P(Mutex, pcb)) {
        return;
    }
    Buffer[pcb.pointer] = pcb.data;
    Enqueue(current_process, pcb);
}

void Consumer(Semaphore* BufferFull, Semaphore* Mutex, int Buffer[],
              int* consumer_pointer, Queue* current_process) {
    PCB pcb = {"c", 0, -1, -1};
    char buf[15];

    consumer_count++;
    sprintf(buf, "%d", consumer_count);

    strcat(pcb.name, buf);

    srand((unsigned)time(NULL));
    pcb.PID = rand() % 1005 + 2020;  // PID范围为 2020-3024

    pcb.pointer = (++(*consumer_pointer)) % 10;
    pcb.data = 0;

    if (!P(BufferFull, pcb)) {
        return;
    }
    if (!P(Mutex, pcb)) {
        return;
    }
    Buffer[pcb.pointer] = pcb.data;
    Enqueue(current_process, pcb);
}

void ExecutePrimitive(Semaphore* BufferEmpty, Semaphore* BufferFull,
                      Semaphore* Mutex, int Buffer[], Queue* current_process) {
    PCB pcb;
    if (Dequeue(current_process, &pcb) == ERROR) {
        printf("\033[47;31mError: No progress is running!\033[0m\n");
    }
    if (pcb.PID >= 1000 && pcb.PID <= 2004) {  // 当前运行进程为生产者进程
        if (V(Mutex)) {  // 有进程阻塞在Mutex信号量上
            PCB tpcb;
            Dequeue(Mutex->queue, &tpcb);
            Buffer[tpcb.pointer] = tpcb.data;
            Enqueue(current_process, tpcb);
        }
        if (V(BufferFull)) {  // 有进程阻塞在BufferFull信号量上
            PCB tpcb;
            Dequeue(BufferFull->queue, &tpcb);
            if (P(Mutex, tpcb)) {
                Buffer[tpcb.pointer] = tpcb.data;
                Enqueue(current_process, tpcb);
            }
        }
    } else if (pcb.PID >= 2020 &&
               pcb.PID <= 3024) {  // 当前运行进程为消费者进程
        if (V(Mutex)) {            // 有进程阻塞在Mutex信号量上
            PCB tpcb;
            Dequeue(Mutex->queue, &tpcb);
            Buffer[tpcb.pointer] = tpcb.data;
            Enqueue(current_process, tpcb);
        }
        if (V(BufferEmpty)) {  // 有进程阻塞在BufferEmpty信号量上
            PCB tpcb;
            Dequeue(BufferEmpty->queue, &tpcb);
            if (P(Mutex, tpcb)) {
                Buffer[tpcb.pointer] = tpcb.data;
                Enqueue(current_process, tpcb);
            }
        }
    }
}