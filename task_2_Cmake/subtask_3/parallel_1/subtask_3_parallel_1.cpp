#include <iostream>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <omp.h>

#define NUM_THREADS 80


double E = 0.00001;


void multiply_matrix_and_vector(double* A, double* x, double* res, long long size_mas) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for (long long i = 0; i < size_mas; i++) {
    res[i] = 0.0;
    for (long long j = 0; j < size_mas; j++) {
      res[i] += A[i * size_mas + j] * x[j];
    }
  }
}


void sub(double* x, double* y, long long size_mas) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for (long long i = 0; i < size_mas; i++) {
    x[i] -= y[i];
  }
}


void mul_by_const(double* x, double CONST, long long size_mas) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for (long long i = 0; i < size_mas; i++) {
    x[i] *= CONST;
  }
}


double L2_norm(double* vector, long long size_mas) {
  double sum = 0;
  #pragma omp parallel for num_threads(NUM_THREADS)
  for (long long i = 0; i < size_mas; i++) {
    sum += (vector[i] * vector[i]);
  }

  return sqrt(sum);
}


bool is_continuing_needed(double* vector_for_criteria, double* b, long long size_mas) {
  double result = L2_norm(vector_for_criteria, size_mas) / L2_norm(b, size_mas);
  // std::cout << std::fixed;
  // std::cout.precision(8);
  // std::cout << "RES" << result << std::endl;
  return result <= E;
}


void solving(double* A, double* x_previous, double *b, long long size_mas, double CONST) {
  double* middle_res = (double*)malloc(sizeof(double) * size_mas);
  
  while (1) {
    multiply_matrix_and_vector(A, x_previous, middle_res, size_mas);
    sub(middle_res, b, size_mas);
    if (is_continuing_needed(middle_res, b, size_mas)) {
      break;
    }
    mul_by_const(middle_res, CONST, size_mas);
    sub(x_previous, middle_res, size_mas);
  }

  free(middle_res);
}


int main() {
  double *A, *x_previous, *b;
  long long size_mas = 11000;

  A = (double*)malloc(sizeof(double) * size_mas * size_mas);
  b = (double*)malloc(sizeof(double) * size_mas);
  x_previous = (double*)malloc(sizeof(double) * size_mas);
  
  
  #pragma omp parallel num_threads(20)
  {
    long long nthreads = omp_get_num_threads();
    long long threadid = omp_get_thread_num();
    long long items_per_thread = size_mas / nthreads;
    long long lb = threadid * items_per_thread;
    long long ub = (threadid == nthreads - 1) ? (size_mas - 1) : (lb + items_per_thread - 1);

    for (long long i = lb; i <= ub; i++) {
      for (long long j = 0; j < size_mas; j++) {
        if (i == j) {
          A[i * size_mas + j] = 2.0;
        } else {
          A[i * size_mas + j] = 1.0;
        }
      }

      b[i] = size_mas + 1;
      x_previous[i] = 0.0;
    }
  }
  
  std::cout << "Start" << std::endl;
  const auto start{std::chrono::steady_clock::now()};
  solving(A, x_previous, b, size_mas, 0.00001);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count() << std::endl;

  free(A);
  free(b);
  free(x_previous);
  return 0;
}

