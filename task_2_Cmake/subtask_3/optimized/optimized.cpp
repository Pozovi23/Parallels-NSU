#include <iostream>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <omp.h>

#define NUM_THREADS 80


double E = 0.00001;


void solving(double* A, double* x_previous, double *b, long long size_mas, double CONST) {
  double* middle_res = (double*)malloc(sizeof(double) * size_mas);
  double err = 10000000000;

  double b_L2 = 0;
  for (long long i = 0; i < size_mas; i++) {
    b_L2 += (b[i] * b[i]);
  }

  b_L2 = sqrt(b_L2);

  while (err > E) {
    double middle_res_L2 = 0;
    #pragma omp parallel for schedule(dynamic, size_mas / NUM_THREADS)
    for (long long i = 0; i < size_mas; i++) {
      middle_res[i] = 0.0;
      for (long long j = 0; j < size_mas; j++) {
        middle_res[i] += A[i * size_mas + j] * x_previous[j];
      }
      middle_res[i] -= b[i];
      double res = (middle_res[i] * middle_res[i]);
      #pragma omp atomic
        middle_res_L2 += res;

      middle_res[i] *= CONST;
      x_previous[i] -= middle_res[i];
    }


    middle_res_L2 = sqrt(middle_res_L2);

    err = middle_res_L2 / b_L2;
    
  }

  free(middle_res);
}


int main() {
  double *A, *x_previous, *b;
  long long size_mas = 11000;

  A = (double*)malloc(sizeof(double) * size_mas * size_mas);
  b = (double*)malloc(sizeof(double) * size_mas);
  x_previous = (double*)malloc(sizeof(double) * size_mas);
  
  
  #pragma omp parallel num_threads(40)
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
  std::cout << 80.48 / elapsed_seconds.count() << std::endl;

  // for (int i = 0; i < size_mas; i++) {
  //   std::cout << x_previous[i] << std::endl;
  // }

  free(A);
  free(b);
  free(x_previous);
  return 0;
}
