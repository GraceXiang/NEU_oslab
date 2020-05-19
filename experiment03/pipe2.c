#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    pid_t pid;
    int fd[2], i;
    int ret = pipe(fd);

    int flags = fcntl(fd[0], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd[0], F_SETFL, flags);
    // 设置读端非阻塞，若父进程读管道时子进程还没向管道中写数据，则不会阻塞等待，以达到同步机制失效的效果。

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

        char buf[1024];
        ret = read(fd[0], buf, sizeof(buf));
        if (ret == -1) {
            perror("Read error");
            exit(1);
        }
        write(STDOUT_FILENO, buf, ret);

        close(fd[0]);
    }
    return 0;
}