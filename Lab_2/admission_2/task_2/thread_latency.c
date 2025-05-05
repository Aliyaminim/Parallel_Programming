#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define MSG_SIZE 64
#define N 10000

char buffer[MSG_SIZE];
int ready = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

double get_time_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void* thread_func(void* arg) {
    for (int i = 0; i < N; ++i) {
        pthread_mutex_lock(&mutex);
        while (!ready)
            pthread_cond_wait(&cond, &mutex);

        // "echo" the message
        strcpy(buffer, buffer);
        ready = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);

    double start = get_time_sec();
    for (int i = 0; i < N; ++i) {
        pthread_mutex_lock(&mutex);
        strcpy(buffer, "ping");
        ready = 1;
        pthread_cond_signal(&cond);

        while (ready)
            pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }
    double end = get_time_sec();

    pthread_join(thread, NULL);
    printf("Avg round-trip time (threads): %.3f µs\n", (end - start) * 1e6 / N);

    return 0;
}
