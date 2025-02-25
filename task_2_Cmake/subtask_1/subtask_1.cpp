#include <cstdlib>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>
#include <chrono>
#include <iostream>


void matrix_vector_product_omp(double *a, double *b, double *c, int m, int n, int NUM_THREADS)
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


double run_parallel(int m, int n, int NUM_THREADS)
{
  double *a, *b, *c;
  // Allocate memory for 2-d array a[m, n]
  a = (double*)malloc(sizeof(double) * m * n);
  b = (double*)malloc(sizeof(double) * n);
  c = (double*)malloc(sizeof(double) * m);

  #pragma omp parallel num_threads(40)
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
  
  std::cout << "Started with " << NUM_THREADS << " threads"<< std::endl;
  const auto start{std::chrono::steady_clock::now()};
  matrix_vector_product_omp(a, b, c, m, n, NUM_THREADS);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count() << std::endl << std::endl;
  
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
  a = (double*)malloc(sizeof(double) * m * n);
  b = (double*)malloc(sizeof(double) * n);
  c = (double*)malloc(sizeof(double) * m);

  #pragma omp parallel num_threads(40)
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
  
  std::cout << "Started serial"<< std::endl;
  const auto start{std::chrono::steady_clock::now()};
  matrix_vector_product(a, b, c, m, n);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count() << std::endl << std::endl;
  
  free(a);
  free(b);
  free(c);
  return elapsed_seconds.count();
}


int main(int argc, char **argv)
{
  int m = 20000, n = 20000; 
  printf("Matrix-vector product (c[m] = a[m, n] * b[n]; m = %d, n = %d)\n", m, n);

  double one_thread_time = 0.0;

  double all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_serial(m, n);
    all_time += curr_time;
  }
  one_thread_time = all_time;
  std::cout << "avg time 1 thread: " << all_time / 10 << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(m, n, 2);
    all_time += curr_time;
  }
  std::cout << "avg time 2 threads: " << all_time / 10 << std::endl;
  std::cout << "S2: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(m, n, 4);
    all_time += curr_time;
  }
  std::cout << "avg time 4 threads: " << all_time / 10 << std::endl;
  std::cout << "S4: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(m, n, 7);
    all_time += curr_time;
  }
  std::cout << "avg time 7 threads: " << all_time / 10 << std::endl;
  std::cout << "S7: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(m, n, 8);
    all_time += curr_time;
  }
  std::cout << "avg time 8 threads: " << all_time / 10 << std::endl;
  std::cout << "S8: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(m, n, 16);
    all_time += curr_time;
  }
  std::cout << "avg time 16 threads: " << all_time / 10 << std::endl;
  std::cout << "S16: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(m, n, 20);
    all_time += curr_time;
  }
  std::cout << "avg time 20 threads: " << all_time / 10 << std::endl;
  std::cout << "S20: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(m, n, 40);
    all_time += curr_time;
  }
  std::cout << "avg time 40 threads: " << all_time / 10 << std::endl;
  std::cout << "S40: " << one_thread_time / all_time << std::endl << std::endl;
  return 0;
}