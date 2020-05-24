#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_INSTRUCTION 20
#define FRAME_NUM 3

typedef struct {     // 分配给进程的页框
    int page_num;    // 存放的某一逻辑页面
    int access_bit;  // 访问位(CLOCK置换策略的参考对象)
    int modified_bit;  // 修改位(改进型CLOCK置换策略需同时参考访问位和修改位)
    // char flag;
} PageFrame;

/*
CLOCK算法命中时需要将access_bit(访问位)置1
*/
bool Hit(int pageNum, PageFrame pagefrmae[]) {
    for (int i = 0; i < FRAME_NUM; i++) {
        if (pagefrmae[i].page_num == pageNum) {
            pagefrmae[i].access_bit = 1;
            return true;
        }
    }
    return false;
}

/*
CLOCK改进算法命中时还需要考虑修改位
*/
bool Hit_Improved(int pageNum, PageFrame pagefrmae[], int modify[]) {
    for (int i = 0; i < FRAME_NUM; i++) {
        if (pagefrmae[i].page_num == pageNum) {
            pagefrmae[i].access_bit = 1;
            pagefrmae[i].modified_bit = modify[i];
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

void CLOCK(int Acess_Series[], PageFrame pagefrmae[]) {
    int hit_num, diseffect = 0;
    int i, j, residence_pointer = 0;

    for (i = 0; i < TOTAL_INSTRUCTION; i++) {
        printf("\033[40;32m\nStart to access logical page %d\033[0m\n",
               Acess_Series[i]);
        if (Hit(Acess_Series[i], pagefrmae)) {
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
                    pagefrmae[j].access_bit = 1;
                    residence_pointer = (residence_pointer + 1) % 3;
                    break;
                }
            }

            // 不存在空闲页面，依据指针residence_pointer的位置进行页面调入
            if (j == FRAME_NUM) {
                while (pagefrmae[residence_pointer].access_bit == 1) {
                    pagefrmae[residence_pointer].access_bit = 0;
                    residence_pointer = (residence_pointer + 1) % 3;
                }
                pagefrmae[residence_pointer].page_num = Acess_Series[i];
                pagefrmae[residence_pointer].access_bit = 1;
                residence_pointer = (residence_pointer + 1) % 3;
            }
            PrintPageFrame(pagefrmae);
        }
    }
    printf(
        "\033[40;34mThe page interrupt rate for CLOCK strategy is "
        "%.2f\033[0m\n",
        diseffect * 1.0 / TOTAL_INSTRUCTION);
}

void CLOCK_Improved(int Acess_Series[], PageFrame pagefrmae[]) {
    int hit_num, diseffect = 0;
    int i, j, residence_pointer = 0;
    int modify[TOTAL_INSTRUCTION];  // 模拟各个页面的修改情况

    srand((unsigned)time(NULL));
    for (i = 0; i < TOTAL_INSTRUCTION; i++) {
        modify[i] = rand() % 2;
    }

    for (i = 0; i < TOTAL_INSTRUCTION; i++) {
        printf("\033[40;32m\nStart to access logical page %d\033[0m\n",
               Acess_Series[i]);
        if (Hit_Improved(Acess_Series[i], pagefrmae, modify)) {
            hit_num++;
            printf("\033[;32mPage hit success!\033[0m\n");
            PrintPageFrame(pagefrmae);
        } else {
            bool flag = false;
            diseffect++;
            printf("\033[;31mA page interrupt occurred!\033[0m\n");

            while (!flag) {
                // 寻找访问位和修改位都为0的物理页面进行置换
                for (j = 0; j < FRAME_NUM; j++) {
                    if (pagefrmae[j].access_bit == 0 &&
                        pagefrmae[j].modified_bit == 0) {
                        pagefrmae[j].page_num = Acess_Series[i];
                        pagefrmae[j].access_bit = 1;
                        pagefrmae[j].modified_bit = modify[i];
                        flag = true;
                        break;
                    }
                }

                // 成功装入内存，置换成功，算法结束
                if (flag) break;

                // 寻找访问位为0，修改位为1的物理页面进行置换
                for (j = 0; j < FRAME_NUM; j++) {
                    if (pagefrmae[j].access_bit == 0 &&
                        pagefrmae[j].modified_bit == 1) {
                        pagefrmae[j].page_num = Acess_Series[i];
                        pagefrmae[j].access_bit = 1;
                        pagefrmae[j].modified_bit = modify[i];
                        flag = true;
                        break;
                    } else {
                        pagefrmae[j].access_bit = 0;
                    }
                }
            }
            PrintPageFrame(pagefrmae);
        }
    }
    printf(
        "\033[40;34mThe page interrupt rate for CLOCK_Improved strategy is "
        "%.2f\033[0m\n",
        diseffect * 1.0 / TOTAL_INSTRUCTION);
}

int main(int argc, const char* argv[]) {
    int i, Acess_Series[TOTAL_INSTRUCTION];  // 页面访问序列
    srand((unsigned)time(NULL));
    for (i = 0; i < TOTAL_INSTRUCTION; i++) {
        Acess_Series[i] = rand() % 5 + 1;
    }

    PageFrame pagefrmae[FRAME_NUM] = {0, 0, 0};

    for (i = 0; i < 2; i++) {  // 循环创建两个子进程
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(1);
        }
        if (pid == 0) break;
    }

    if (i == 0) {
        printf("CLOCK:");
        CLOCK(Acess_Series, pagefrmae);  // 时钟置换算法
    } else if (i == 1) {
        sleep(2);  // 等待子进程1结束
        printf("CLOCK_Improved:");
        CLOCK_Improved(Acess_Series, pagefrmae);  // 改进的时钟置换算法
    } else {
        for (i = 0; i < 2; i++) wait(NULL);  // 阻塞回收两个子进程
    }

    return 0;
}