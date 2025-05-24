#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <chrono>

void compute_with_pointers(int N, double *a, double *b, double *c, double &sum) {

    for (int i = 0; i < N/2; ++i) {
        int index = (i * 2); // Irregular pattern access
        c[i] = std::log1p(a[index]) * std::sqrt(b[index]) + std::sin(a[i]  * 0.01); // Heavy computations
    }

    for (int i = 0; i < N; ++i) {
        sum += c[i];
    }
}

int main() {
    const int N = 1000000;
    double *a = new double[N];
    double *b = new double[N];
    double *c = new double[N];
    double sum = 0.0;

    for (int i = 0; i < N; ++i) {
        a[i] = 1.0;
        b[i] = 2.0;
        c[i] = 0.0;
    }


    auto start = std::chrono::high_resolution_clock::now();
    compute_with_pointers(N, a, b, c, sum);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Output sum: " << sum << std::endl;
    std::cout << "Time: " << elapsed.count() << " seconds" << std::endl;

    delete[] a;
    delete[] b;
    delete[] c;

    return 0;
}



