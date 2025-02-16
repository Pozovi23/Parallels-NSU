#include <cstdlib>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>
#include <chrono>
#define NUM_THREADS 40


void matrix_vector_product_omp(double *a, double *b, double *c, int m, int n)
{
  #pragma omp parallel num_threads(NUM_THREADS)
  {
    int nthreads = omp_get_num_threads();
    int threadid = omp_get_thread_num(); // идентификатор потока
    int items_per_thread = m / nthreads;
    int lb = threadid * items_per_thread; // нижняя граница индекса для текущего потока
    int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1); // верхняя граница индекса для текущего потока
    for (int i = lb; i <= ub; i++) {
      c[i] = 0.0;
      for (int j = 0; j < n; j++)
        c[i] += a[i * n + j] * b[j];
    }
  }
}


double run_parallel(int m, int n)
{
  double *a, *b, *c;
  // Allocate memory for 2-d array a[m, n]
  a = (double*)malloc(sizeof(double) * m * n);
  b = (double*)malloc(sizeof(double) * n);
  c = (double*)malloc(sizeof(double) * m);

  #pragma omp parallel num_threads(NUM_THREADS)
  {
    int nthreads = omp_get_num_threads();
    int threadid = omp_get_thread_num();
    int items_per_thread = m / nthreads;
    int lb = threadid * items_per_thread;
    int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);
    for (int i = lb; i <= ub; i++) {
      for (int j = 0; j < n; j++) {
        a[i * n + j] = i + j;
      }
      c[i] = 0.0;
    }

    for (int j = 0; j < n; j++) {
      b[j] = j;
    }
  }
  
  const auto start{std::chrono::steady_clock::now()};
  matrix_vector_product_omp(a, b, c, m, n);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};

  printf("Elapsed time (parallel): %.6f sec.\n", elapsed_seconds.count());
  free(a);
  free(b);
  free(c);
  return elapsed_seconds.count();
}


void matrix_vector_product(double *a, double *b, double *c, int m, int n)
{
  for (int i = 0; i < m; i++) {
    c[i] = 0.0;
    for (int j = 0; j < n; j++)
      c[i] += a[i * n + j] * b[j];
  }
}


double run_serial(int m, int n) {
  double *a, *b, *c;
  // Allocate memory for 2-d array a[m, n]
  a = (double*)malloc(sizeof(double) * m * n);
  b = (double*)malloc(sizeof(double) * n);
  c = (double*)malloc(sizeof(double) * m);

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      a[i * n + j] = i + j;
    }
    c[i] = 0.0;
  }

  for (int j = 0; j < n; j++) {
    b[j] = j;
  }
  
  const auto start{std::chrono::steady_clock::now()};
  matrix_vector_product(a, b, c, m, n);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};

  printf("Elapsed time (serial): %.6f sec.\n", elapsed_seconds.count());
  free(a);
  free(b);
  free(c);
  return elapsed_seconds.count();
}


int main(int argc, char **argv)
{
  int m = 40000, n = 40000; 
  printf("Matrix-vector product (c[m] = a[m, n] * b[n]; m = %d, n = %d)\n", m, n);
  printf("Memory used: %" PRIu64 " MiB\n", ((m * n + m + n) * sizeof(double)) >> 20);  

  double tserial = run_serial(m, n);
  double tparallel = run_parallel(m, n);
  printf("Speedup: %.2f\n", tserial / tparallel);
  return 0;
}