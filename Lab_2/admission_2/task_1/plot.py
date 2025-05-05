import matplotlib.pyplot as plt

# Данные
sizes = [1000, 10000, 50000, 100000, 500000, 1000000, 5000000, 10000000]
sequential = [4.11e-05, 5.2e-04, 3.2e-03, 6.5e-03, 3.6e-02, 7.6e-02, 4.3e-01, 9.0e-01]
parallel =   [2.0e-04, 6.9e-04, 2.8e-03, 5.9e-03, 2.9e-02, 5.1e-02, 3.4e-01, 6.8e-01]

# Настройка графика
plt.figure(figsize=(10, 6))
plt.plot(sizes, sequential, marker='o', label='Sequential std::sort', linewidth=2)
plt.plot(sizes, parallel, marker='s', label='Parallel sort', linewidth=2)

plt.xscale('log')
plt.yscale('log')

plt.xlabel('Array Size (log scale)', fontsize=12)
plt.ylabel('Execution Time (seconds, log scale)', fontsize=12)
plt.title('Sorting Time vs Input Size', fontsize=14)

plt.grid(True, which="both", ls="--", lw=0.5)
plt.legend(fontsize=10)
plt.tight_layout()
plt.savefig("sorting_performance.png", dpi=300)  # Сохраняет в файл
plt.show()
