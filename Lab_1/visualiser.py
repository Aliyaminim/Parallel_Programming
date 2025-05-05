import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

with open("solution_full.txt") as f:
    blocks = f.read().strip().split("\n\n")

data = [np.array([[float(x) for x in line.split()] for line in block.splitlines()])
        for block in blocks]

T_vals = sorted(set(row[0] for block in data for row in block))
X_vals = sorted(set(row[1] for block in data for row in block))

T = np.array(T_vals)
X = np.array(X_vals)
U = np.array([[row[2] for row in block] for block in data])

T_grid, X_grid = np.meshgrid(T, X, indexing='ij')

fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(X_grid, T_grid, U, cmap='viridis')

ax.set_xlabel("x")
ax.set_ylabel("t")
ax.set_zlabel("u(t,x)")
plt.title("Решение уравнения u(t,x)")
plt.savefig("solution_plot.png")

