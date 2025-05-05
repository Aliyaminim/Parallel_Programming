#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

double phi(double x) {
    return sin(M_PI * x);;
}

double psi(double t) {
    return t;
}

double f(double t, double x) {
    return x*t;
}

void solve_transport_equation(double a, double T, double X, int K, int M) {
    auto start = std::chrono::high_resolution_clock::now();
    double tau = T / K;
    double h = X / M;

    std::vector<std::vector<double>> u(K + 1, std::vector<double>(M + 1));

    // Инициализация начальных и граничных условий
    for (int m = 0; m <= M; ++m) {
        u[0][m] = phi(m * h);  // Начальное условие u(0,x)=φ(x)
    }
    for (int k = 0; k <= K; ++k) {
        u[k][0] = psi(k * tau);  // Граничное условие u(t,0)=ψ(t)
    }

    // Численное решение
    for (int k = 0; k < K; ++k) {
        for (int m = 1; m < M; ++m) {
            u[k+1][m] = 0.5 * (u[k][m+1] + u[k][m-1]) -
                        (a * tau / (2 * h)) * (u[k][m+1] - u[k][m-1]) +
                        tau * f(k * tau, m * h);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Sequential time: " << elapsed.count() << " sec\n";

    std::ofstream out("solution_full_serial.txt");
    for (int k = 0; k <= K; ++k) {
        for (int m = 0; m <= M; ++m) {
            out << k * tau << " " << m * h << " " << u[k][m] << "\n";
        }
        out << "\n";  // Пустая строка между слоями
    }
    out.close();
}

int main() {
    double a = 1.0;  // Скорость переноса
    double T = 1.0;   // Время
    double X = 1.0;   // Пространство
    int K = 100;      // Шаги по времени
    int M = 100;      // Шаги по пространству

    solve_transport_equation(a, T, X, K, M);
    return 0;
}