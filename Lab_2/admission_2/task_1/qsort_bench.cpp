#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <random>
#include <chrono>
#include <fstream>

void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        temp[k++] = (arr[i] < arr[j]) ? arr[i++] : arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (int l = 0; l < k; ++l) arr[left + l] = temp[l];
}

void parallel_merge_sort(std::vector<int>& arr, int left, int right, int depth = 0) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;

    if (depth < std::thread::hardware_concurrency()) {
        std::thread t1(parallel_merge_sort, std::ref(arr), left, mid, depth + 1);
        parallel_merge_sort(arr, mid + 1, right, depth + 1);
        t1.join();
    } else {
        parallel_merge_sort(arr, left, mid, depth + 1);
        parallel_merge_sort(arr, mid + 1, right, depth + 1);
    }

    merge(arr, left, mid, right);
}

void run_experiment(std::ofstream& out, int size) {
    std::vector<int> data(size);
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, size);
    for (int& x : data) x = dis(gen);

    std::vector<int> copy = data;

    auto start = std::chrono::high_resolution_clock::now();
    std::sort(copy.begin(), copy.end());
    auto end = std::chrono::high_resolution_clock::now();
    double sequential_time = std::chrono::duration<double>(end - start).count();

    copy = data;
    start = std::chrono::high_resolution_clock::now();
    parallel_merge_sort(copy, 0, copy.size() - 1);
    end = std::chrono::high_resolution_clock::now();
    double parallel_time = std::chrono::duration<double>(end - start).count();

    std::cout << "Size: " << size << ", Sequential: " << sequential_time
              << " s, Parallel: " << parallel_time << " s\n";
    out << size << "," << sequential_time << "," << parallel_time << "\n";
}

int main() {
    std::ofstream results("sort_times.csv");
    results << "Size,Sequential,Parallel\n";

    std::vector<int> sizes = {
        1000, 10000, 50000, 100000,
        500000, 1000000, 5000000, 10000000
    };

    for (int size : sizes) {
        run_experiment(results, size);
    }

    results.close();
    return 0;
}
