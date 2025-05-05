#include <iostream>
#include <vector>
#include <mpi.h>
#include <cmath>
#include <fstream>

double phi(double x) {
    return sin(M_PI * x);
}

double psi(double t) {
    return t;
}

double f(double t, double x) {
    return x * t;
}

void solve_parallel(double a, double T, double X, int K, int M, int rank, int size) {
    double start_time = 0.0;
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    double tau = T / K;
    double h = X / M;

    // Разбиение по процессам с учётом остатка
    int base = M / size;
    int rem = M % size;

    int local_M = base + (rank < rem ? 1 : 0); // сколько точек у процесса
    int start_m = rank * base + std::min(rank, rem); // индекс первой точки

    std::vector<std::vector<double>> u(K + 1, std::vector<double>(local_M + 2));

    // Начальное условие: u(0, x) = phi(x)
    for (int m = 0; m < local_M; ++m) {
        double x = (start_m + m) * h;
        u[0][m + 1] = phi(x);
    }

    // Граничное условие: u(t, 0) = psi(t) — только у процесса, содержащего x=0
    if (start_m == 0) {
        for (int k = 0; k <= K; ++k) {
            u[k][1] = psi(k * tau); // значение в x=0 (m=0), соответствует локальному m=0+1
        }
    }

    // Временной цикл
    for (int k = 0; k < K; ++k) {
        // Обмен граничными значениями
        if (rank > 0) {
            MPI_Sendrecv(&u[k][1], 1, MPI_DOUBLE, rank - 1, 0,
                         &u[k][0], 1, MPI_DOUBLE, rank - 1, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            MPI_Sendrecv(&u[k][local_M], 1, MPI_DOUBLE, rank + 1, 0,
                         &u[k][local_M + 1], 1, MPI_DOUBLE, rank + 1, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Вычисление нового слоя
        for (int m = 1; m <= local_M; ++m) {
            double x = (start_m + m - 1) * h;
            double t = k * tau;
            u[k + 1][m] = 0.5 * (u[k][m + 1] + u[k][m - 1]) -
                          (a * tau / (2 * h)) * (u[k][m + 1] - u[k][m - 1]) +
                          tau * f(t, x);
        }
    }

    if (rank == 0) {
        double end_time = MPI_Wtime();
        std::cout << "Execution time: " << (end_time - start_time) << " seconds\n";
    }

    // Подготовка к сбору данных
    std::vector<int> counts(size), displs(size);
    for (int p = 0; p < size; ++p) {
        counts[p] = ((M / size) + (p < rem ? 1 : 0));
        displs[p] = (p > 0 ? displs[p - 1] + counts[p - 1] : 0);
    }

    // Собираем все временные слои
    if (rank == 0) {
        std::ofstream out("solution_full.txt");
        std::vector<double> recvbuf(M);

        for (int k = 0; k <= K; ++k) {
            MPI_Gatherv(&u[k][1], local_M, MPI_DOUBLE,
                        recvbuf.data(), counts.data(), displs.data(),
                        MPI_DOUBLE, 0, MPI_COMM_WORLD);

            for (int m = 0; m < M; ++m) {
                out << k * tau << " " << m * h << " " << recvbuf[m] << "\n";
            }
            out << "\n";
        }
        out.close();
    } else {
        for (int k = 0; k <= K; ++k) {
            MPI_Gatherv(&u[k][1], local_M, MPI_DOUBLE,
                        nullptr, nullptr, nullptr,
                        MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 1.0, T = 1.0, X = 1.0;
    int K = 100, M = 100;

    solve_parallel(a, T, X, K, M, rank, size);

    MPI_Finalize();
    return 0;
}

