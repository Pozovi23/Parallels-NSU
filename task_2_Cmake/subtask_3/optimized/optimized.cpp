#include <iostream>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <omp.h>


double E = 0.00001;


void solving(double* A, double* x_previous, double *b, long long size_mas, double CONST, int NUM_THREADS) {
  double* middle_res = (double*)malloc(sizeof(double) * size_mas);
  double err = 10000000000;

  double b_L2 = 0;
  #pragma omp parallel for schedule(static, size_mas / NUM_THREADS)
  for (long long i = 0; i < size_mas; i++) {
    b_L2 += (b[i] * b[i]);
  }

  b_L2 = sqrt(b_L2);

  while (err > E) {
    double middle_res_L2 = 0;
    #pragma omp parallel for schedule(static, size_mas / NUM_THREADS)
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


double one_iteration(int NUM_THREADS) {
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

  std::cout << "Started with " << NUM_THREADS << " threads"<< std::endl;
  const auto start{std::chrono::steady_clock::now()};
  solving(A, x_previous, b, size_mas, 0.00001, NUM_THREADS);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count() << std::endl << std::endl;
  // for (int i = 0; i < size_mas; i++) {
  //   std::cout << x_previous[i] << std::endl;
  // }

  free(A);
  free(b);
  free(x_previous);
  return elapsed_seconds.count();
}




int main() {
  double one_thread_time = 320.25;

  double all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(2);
    all_time += curr_time;
  }
  std::cout << "avg time 2 threads: " << all_time / 10 << std::endl;
  std::cout << "S2: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(4);
    all_time += curr_time;
  }
  std::cout << "avg time 4 threads: " << all_time / 10 << std::endl;
  std::cout << "S4: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(7);
    all_time += curr_time;
  }
  std::cout << "avg time 7 threads: " << all_time / 10 << std::endl;
  std::cout << "S7: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(8);
    all_time += curr_time;
  }
  std::cout << "avg time 8 threads: " << all_time / 10 << std::endl;
  std::cout << "S8: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(16);
    all_time += curr_time;
  }
  std::cout << "avg time 16 threads: " << all_time / 10 << std::endl;
  std::cout << "S16: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(20);
    all_time += curr_time;
  }
  std::cout << "avg time 20 threads: " << all_time / 10 << std::endl;
  std::cout << "S20: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(40);
    all_time += curr_time;
  }
  std::cout << "avg time 40 threads: " << all_time / 10 << std::endl;
  std::cout << "S40: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = one_iteration(80);
    all_time += curr_time;
  }
  std::cout << "avg time 80 threads: " << all_time / 10 << std::endl;
  std::cout << "S80: " << one_thread_time / all_time << std::endl << std::endl;
  return 0;
}
