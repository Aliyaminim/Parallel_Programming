#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MAX_THREADS 8
#define EPS 1e-6

typedef struct {
    double a, b;
    int steps;
} Task;

typedef struct {
    double a, b;
    double result;
} ThreadArg;

pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;

double f(double x) {
    return sin(1.0 / x);
}

// Адаптивное численное интегрирование по методу трапеций
double integrate(double a, double b, double eps) {
    int n = 1;
    double h = (b - a);
    double prev = h * (f(a) + f(b)) / 2.0;
    while (1) {
        double sum = 0.0;
        double step = h / n;
        for (int i = 1; i < n; ++i) {
            sum += f(a + i * step);
        }
        double current = step * (f(a) + 2 * sum + f(b)) / 2.0;
        if (fabs(current - prev) < eps) {
            return current;
        }
        prev = current;
        n *= 2;
    }
}

#define CHUNK_COUNT 100000
Task tasks[CHUNK_COUNT];
int task_index = 0;

void* thread_func(void* arg) {
    ThreadArg* targs = (ThreadArg*)arg;
    double sum = 0.0;

    while (1) {
        pthread_mutex_lock(&task_mutex);
        if (task_index >= CHUNK_COUNT) {
            pthread_mutex_unlock(&task_mutex);
            break;
        }
        Task task = tasks[task_index++];
        pthread_mutex_unlock(&task_mutex);

        double local_result = integrate(task.a, task.b, EPS);
        sum += local_result;
    }

    targs->result = sum;
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Usage: %s <a> <b> <num_threads>\n", argv[0]);
        return 1;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    double a = atof(argv[1]);
    double b = atof(argv[2]);
    int num_threads = atoi(argv[3]);
    if (num_threads > MAX_THREADS) num_threads = MAX_THREADS;

    // Заполняем задания
    double step = (b - a) / CHUNK_COUNT;
    for (int i = 0; i < CHUNK_COUNT; ++i) {
        double x0 = a + i * step;
        double x1 = x0 + step;
        tasks[i].a = x0;
        tasks[i].b = x1;
    }

    pthread_t threads[MAX_THREADS];
    ThreadArg args[MAX_THREADS];

    for (int i = 0; i < num_threads; ++i) {
        args[i].a = a;
        args[i].b = b;
        args[i].result = 0.0;
        pthread_create(&threads[i], NULL, thread_func, &args[i]);
    }

    double total = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        total += args[i].result;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Elapsed time: %.6f seconds\n", elapsed);

    printf("Integral from %f to %f = %.10f\n", a, b, total);
    return 0;
}
