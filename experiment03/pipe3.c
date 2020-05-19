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

    int flags = fcntl(fd[1], F_GETLK);
    flags |= F_ULOCK;
    fcntl(fd[1], F_SETLK, flags);
    //设置写端非阻塞
    // flags = fcntl(fd[1], F_GETFL);
    // flags |= O_NONBLOCK;
    // fcntl(fd[1], F_SETFL, flags);

    for (i = 0; i < 2; i++) {  // 循环创建2个子进程
        pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(1);
        }
        if (pid == 0) break;
    }

    if (i <= 1) {
        close(fd[0]);

        char str[409600];
        if (i == 0) {
            for (i = 0; i < 204800; i++) {
                str[i] = '1';
            }
        } else {
            for (i = 0; i < 204800; i++) {
                str[i] = '2';
            }
        }
        lockf(fd[1], 1, 0);
        write(fd[1], str, strlen(str));
        lockf(fd[1], 0, 0);
        close(fd[1]);

    } else {
        close(fd[1]);

        char buf[512000];
        while (1) {
            sleep(1);
            ret = read(fd[0], buf, sizeof(buf));
            write(STDOUT_FILENO, buf, ret);
        }

        close(fd[0]);
    }
    return 0;
}