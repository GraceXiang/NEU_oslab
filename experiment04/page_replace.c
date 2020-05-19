#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_INSTRUCTION 12
#define FRAME_NUM 3

typedef struct {         // 分配给进程的页框
    int page_num;        // 存放的某一页面
    int interview_time;  // 访问时间(LRU置换策略的参考对象，数值越大访问越早，优先被置换)
    // char flag;
} PageFrame;

/*
命中返回true
缺页返回false
*/
bool FindPage(int pageNum, PageFrame pagefrmae[]) {
    for (int i = 0; i < FRAME_NUM; i++) {
        if (pagefrmae[i].page_num == pageNum) {
            return true;
        }
    }
    return false;
}

void PrintPageFrame(PageFrame pagefrmae[]) {
    printf("|--------|\n");
    for (int i = 0; i < FRAME_NUM; i++) {
        printf("|   %2d   |\n", pagefrmae[i].page_num);
        printf("|--------|\n");
    }
}

void FIFO(int Acess_Series[], PageFrame pagefrmae[]) {
    int hit_num = 0, diseffect = 0;
    int i, j, residence_pointer = 0;  // 置换指针(FIFO算法参考对象)
    for (i = 0; i < TOTAL_INSTRUCTION; i++) {
        printf("\033[40;32m\nStart to access logical page %d\033[0m\n",
               Acess_Series[i]);
        if (FindPage(Acess_Series[i], pagefrmae)) {
            hit_num++;
            printf("\033[;32mPage hit success!\033[0m\n");
            PrintPageFrame(pagefrmae);
        } else {
            diseffect++;
            printf("\033[;31mA page interrupt occurred!\033[0m\n");

            // 实际分配的物理页面中，还存在空闲页面
            for (j = 0; j < FRAME_NUM; j++) {
                if (pagefrmae[j].page_num == 0) {
                    pagefrmae[j].page_num = Acess_Series[i];
                    break;
                }
            }

            // 不存在空闲页面，依据指针residence_pointer的位置进行页面调入
            if (j == 3) {
                pagefrmae[(residence_pointer++) % 3].page_num = Acess_Series[i];
            }
            PrintPageFrame(pagefrmae);
        }
    }
    printf(
        "\033[40;34mThe page interrupt rate for FIFO strategy is %.2f\033[0m\n",
        diseffect * 1.0 / TOTAL_INSTRUCTION);
}

void LRU(int Acess_Series[], PageFrame pagefrmae[]) {
    sleep(1);  // 等待子进程1结束
    int hit_num = 0, diseffect = 0;
    int i, j = 0;
    for (i = 0; i < TOTAL_INSTRUCTION; i++) {
        printf("\033[40;32m\nStart to access logical page %d\033[0m\n",
               Acess_Series[i]);
        if (FindPage(Acess_Series[i], pagefrmae)) {
            hit_num++;
            printf("\033[;32mPage hit success!\033[0m\n");

            // 命中的物理页面最久未访问时间重置为0，其它物理页面时间+1
            for (j = 0; j < FRAME_NUM; j++) {
                if (Acess_Series[i] == pagefrmae[j].page_num) {
                    pagefrmae[j].interview_time = 0;
                } else if (pagefrmae[j].page_num != 0){
                    pagefrmae[j].interview_time++;
                }
            }

            PrintPageFrame(pagefrmae);
        } else {
            diseffect++;
            printf("\033[;31mA page interrupt occurred!\033[0m\n");

            // 实际分配的物理页面中，还存在空闲页面
            for (j = 0; j < FRAME_NUM; j++) {
                if (pagefrmae[j].page_num == 0) {
                    pagefrmae[j].page_num = Acess_Series[i];
                    break;
                } else {
                    pagefrmae[j].interview_time++;
                }
            }

            // 不存在空闲页面，依据interview_time的大小进行页面调入
            if (j == 3) {
                int second_max = pagefrmae[0].interview_time > pagefrmae[1].interview_time ? 0 : 1;
                int max = pagefrmae[second_max].interview_time > pagefrmae[2].interview_time ?second_max : 2;
                pagefrmae[max].page_num = Acess_Series[i];
                pagefrmae[max].interview_time = 0;
            }
            PrintPageFrame(pagefrmae);
        }
    }
    printf(
        "\033[40;34mThe page interrupt rate for LRU strategy is %.2f\033[0m\n",
        diseffect * 1.0 / TOTAL_INSTRUCTION);
}

int main(int argc, const char* argv[]) {
    int i, Acess_Series[TOTAL_INSTRUCTION];  // 页面访问序列
    srand((unsigned)time(NULL));
    for (i = 0; i < TOTAL_INSTRUCTION; i++) {
        Acess_Series[i] = rand() % 5 + 1;
    }

    PageFrame pagefrmae[FRAME_NUM] = {0, 0};

    // pagefrmae[0].page_num = 0;
    // pagefrmae[1].page_num = 0;
    // pagefrmae[2].page_num = 0;

    for (i = 0; i < 2; i++) {  // 循环创建两个子进程
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(1);
        }
        if (pid == 0) break;
    }

    if (i == 0) {
        printf("FIFO:");
        FIFO(Acess_Series, pagefrmae);  // 先进先出算法
    } else if (i == 1) {
        printf("LRU:");
        LRU(Acess_Series, pagefrmae);  // 最近最久未使用算法
    } else {
        for (i = 0; i < 2; i++) wait(NULL);  // 阻塞回收两个个子进程
    }

    return 0;
}