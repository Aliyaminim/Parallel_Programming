#include <iostream>
#include <iomanip>
#include <mpi.h>

constexpr long N = 100'000'000;  // Общее количество прямоугольников

// вычисление интеграла методом средних прямоугольников
double calculate_partial_pi(int start, int end, double step) {
    double sum = 0.0;
    for (int i = start; i < end; ++i) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    return sum;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time = 0.0;
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    const double step = 1.0 / static_cast<double>(N);
    const int chunk_size = N / size;
    const int start = rank * chunk_size;
    const int end = (rank == size - 1) ? N : start + chunk_size;

    const double partial_sum = calculate_partial_pi(start, end, step);
    double pi = 0.0;
    MPI_Reduce(&partial_sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        pi *= step;
        const double end_time = MPI_Wtime();

        std::cout << std::setprecision(16);
        std::cout << "Calculated Pi = " << pi << std::endl;
        std::cout << "Error = " << pi - 3.14159265358979323846 << std::endl;
        std::cout << "Time = " << std::fixed << end_time - start_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}