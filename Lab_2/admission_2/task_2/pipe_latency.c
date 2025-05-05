#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define MSG_SIZE 64
#define N 10000

double get_time_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main() {
    int pipe1[2]; // parent -> child
    int pipe2[2]; // child -> parent

    pipe(pipe1);
    pipe(pipe2);

    pid_t pid = fork();

    if (pid == 0) {
        // Child
        char buf[MSG_SIZE];
        for (int i = 0; i < N; ++i) {
            read(pipe1[0], buf, MSG_SIZE);
            write(pipe2[1], buf, MSG_SIZE);
        }
        exit(0);
    } else {
        // Parent
        char msg[MSG_SIZE] = "ping";
        char buf[MSG_SIZE];

        double start = get_time_sec();
        for (int i = 0; i < N; ++i) {
            write(pipe1[1], msg, MSG_SIZE);
            read(pipe2[0], buf, MSG_SIZE);
        }
        double end = get_time_sec();

        wait(NULL);
        printf("Avg round-trip time (pipe): %.3f µs\n", (end - start) * 1e6 / N);
    }

    return 0;
}
