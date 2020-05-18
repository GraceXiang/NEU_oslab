#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    pid_t pid;
    int fd[2], i;
    int ret = pipe(fd);

    for (i = 0; i < 3; i++) {  // 循环创建3个子进程
        pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(1);
        }
        if (pid == 0) break;
    }

    if (i <= 2) {
        close(fd[0]);
        char* str;
        if (i == 0) {
            str = "Child process1 is sending a message!\n";
        } else if (i == 1) {
            str = "Child process2 is sending a message!\n";
        } else {
            str = "Child process3 is sending a message!\n";
        }

        write(fd[1], str, strlen(str));

        close(fd[1]);
    } else {
        close(fd[1]);
        for (i = 0; i < 3; i++) wait(NULL);  // 阻塞回收三个子进程

        char buf[1024];
        ret = read(fd[0], buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);

        close(fd[0]);
    }
    return 0;
}