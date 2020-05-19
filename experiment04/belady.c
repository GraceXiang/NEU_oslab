#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_INSTRUCTION 12
/*
Belady现象: 在内存中分配3个页面，缺页率为0.75
           在内存中分配4个页面，缺页率为0.83
*/
#define FRAME_NUM 3
//#define FRAME_NUM 4

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
    int hit_num, diseffect = 0;
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
            if (j == FRAME_NUM) {
                pagefrmae[(residence_pointer++) % FRAME_NUM].page_num = Acess_Series[i];
            }
            PrintPageFrame(pagefrmae);
        }
    }
    printf(
        "\033[40;34mThe page interrupt rate for FIFO strategy is %.2f\033[0m\n",
        diseffect * 1.0 / TOTAL_INSTRUCTION);
}

int main(int argc, const char* argv[]) {
    int i = 0;
    // 页面访问序列
    int Acess_Series[TOTAL_INSTRUCTION] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

    PageFrame pagefrmae[FRAME_NUM] = {0, 0};

    FIFO(Acess_Series, pagefrmae);  // 最近最久未使用算法

    return 0;
}