import numpy as np
import time
from numba import njit
from numba.openmp import openmp_context as openmp

# Function to multiply matrices using 1D arrays and explicit loops
def multiply_matrices_flat(A, B, N):
    result = np.zeros(N * N)
    for i in range(N):
        for j in range(N):
            sum_val = 0.0
            for k in range(N):
                sum_val += A[i * N + k] * B[k * N + j]
            result[i * N + j] = sum_val
    return result

@njit
def multiply_matrices_gpu(A, B, N):
    result = np.zeros(N * N)
    with openmp("target map(to: result) map(to: A) map(to: B)"):
        with openmp("parallel for"):
            for i in range(N):
                for j in range(N):
                    sum_val = 0.0
                    for k in range(N):
                        sum_val += A[i * N + k] * B[k * N + j]
                    result[i * N + j] = sum_val
    return result

# Size of the matrices (you can adjust to test performance)
N = 500

# Fix the seed for reproducibility
np.random.seed(42)

# Create random matrices as 1D arrays
A = np.random.rand(N * N)
B = np.random.rand(N * N)

# Record start time
start_time = time.time()

# Perform multiplication
y = multiply_matrices_flat(A, B, N)

# Record end time
end_time = time.time()

# Display execution time
print(f"Multiplication time: {end_time - start_time:.4f} seconds")

# Print the reduction (sum of all elements) to verify result
print(f"Reduction (sum of result matrix): {np.sum(y):.4f}")

### GPU

# Record start time
start_time = time.time()

# Perform multiplication
y_gpu = multiply_matrices_gpu(A, B, N)

# Record end time
end_time = time.time()

print("GPU")
# Display execution time
print(f"Multiplication time: {end_time - start_time:.4f} seconds")

# Print the reduction (sum of all elements) to verify result
print(f"Reduction (sum of result matrix): {np.sum(y_gpu):.4f}")