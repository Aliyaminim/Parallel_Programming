#include <iostream>
#include <vector>
#include <chrono>
#include <mpi.h>

// Функция для измерения времени передачи
void measure_communication(int rank, int partner_rank, int message_size, int iterations) {
    std::vector<char> buffer(message_size);
    double total_time = 0.0;

    // Синхронизация перед началом измерений
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < iterations; ++i) {
        if (rank == 0) {
            // Процесс 0 отправляет и получает
            double start = MPI_Wtime();
            MPI_Send(buffer.data(), message_size, MPI_BYTE, partner_rank, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer.data(), message_size, MPI_BYTE, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            double end = MPI_Wtime();
            total_time += (end - start);
        }
        else if (rank == 1) {
            // Процесс 1 получает и отправляет
            MPI_Recv(buffer.data(), message_size, MPI_BYTE, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer.data(), message_size, MPI_BYTE, partner_rank, 0, MPI_COMM_WORLD);
        }
    }

    if (rank == 0) {
        double avg_time = total_time / (2 * iterations); // Учитываем туда-обратно
        double bandwidth = (message_size * 8) / (avg_time * 1e6); // Мбит/с

        std::cout << "Message size: " << message_size << " bytes" << std::endl;
        std::cout << "Average round-trip time: " << avg_time * 1e6 << " μs" << std::endl;
        std::cout << "Estimated bandwidth: " << bandwidth << " Mbps" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            std::cerr << "This program requires at least 2 processes" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    const int partner_rank = (rank == 0) ? 1 : 0;
    const int iterations = 100; // Количество итераций для усреднения

    // Тестируем разные размеры сообщений
    std::vector<int> message_sizes = {0, 1, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576};

    if (rank == 0) {
        std::cout << "MPI Communication Benchmark" << std::endl;
        std::cout << "Number of iterations: " << iterations << std::endl;
        std::cout << "========================================" << std::endl;
    }

    for (int size : message_sizes) {
        measure_communication(rank, partner_rank, size, iterations);
    }

    MPI_Finalize();
    return 0;
}