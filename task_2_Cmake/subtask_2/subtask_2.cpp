#include <cstdlib>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>
#include <chrono>
#include <math.h>
#include <iostream>


double func(double x)
{
  return exp(-x * x); 
}


double integrate_omp(double (*func)(double), double a, double b, int n, int NUM_THREADS)
{
  double h = (b - a) / n;
  double sum = 0.0;
  #pragma omp parallel num_threads(NUM_THREADS)
  {
    int nthreads = omp_get_num_threads();
    int threadid = omp_get_thread_num();
    int items_per_thread = n / nthreads;
    int lb = threadid * items_per_thread;
    int ub = (threadid == nthreads - 1) ? (n - 1) : (lb + items_per_thread - 1);
    double sumloc = 0.0;
    for (int i = lb; i <= ub; i++)
      sumloc += func(a + h * (i + 0.5));  
    #pragma omp atomic
    sum += sumloc;
  }
  sum *= h;
  return sum;
}


double integrate(double (*func)(double), double a, double b, int n)
{
  double h = (b - a) / n;
  double sum = 0.0;
  for (int i = 0; i < n; i++)
    sum += func(a + h * (i + 0.5));
  sum *= h;
  return sum;
}


const double a = -4.0;
const double b = 4.0;
const int nsteps = 40000000;

double run_parallel(int NUM_THREADS)
{
  std::cout << "Started with " << NUM_THREADS << " threads"<< std::endl;
  const auto start{std::chrono::steady_clock::now()};
  double res = integrate_omp(func, a, b, nsteps, NUM_THREADS);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count() << std::endl << std::endl;

  return elapsed_seconds.count();
}


double run_serial()
{
  std::cout << "Started serial"<< std::endl;
  const auto start{std::chrono::steady_clock::now()};
  double res = integrate(func, a, b, nsteps);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count() << std::endl << std::endl;

  return elapsed_seconds.count();
}


int main(int argc, char **argv)
{
  printf("Integration f(x) on [%.12f, %.12f], nsteps = %d\n", a, b, nsteps);

  double one_thread_time = 0.0;

  double all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_serial();
    all_time += curr_time;
  }
  one_thread_time = all_time;
  std::cout << "avg time 1 thread: " << all_time / 10 << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(2);
    all_time += curr_time;
  }
  std::cout << "avg time 2 threads: " << all_time / 10 << std::endl;
  std::cout << "S2: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(4);
    all_time += curr_time;
  }
  std::cout << "avg time 4 threads: " << all_time / 10 << std::endl;
  std::cout << "S4: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(7);
    all_time += curr_time;
  }
  std::cout << "avg time 7 threads: " << all_time / 10 << std::endl;
  std::cout << "S7: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(8);
    all_time += curr_time;
  }
  std::cout << "avg time 8 threads: " << all_time / 10 << std::endl;
  std::cout << "S8: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(16);
    all_time += curr_time;
  }
  std::cout << "avg time 16 threads: " << all_time / 10 << std::endl;
  std::cout << "S16: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(20);
    all_time += curr_time;
  }
  std::cout << "avg time 20 threads: " << all_time / 10 << std::endl;
  std::cout << "S20: " << one_thread_time / all_time << std::endl << std::endl;


  all_time = 0.0;
  for (int i = 0; i < 10; i++) {
    double curr_time = run_parallel(40);
    all_time += curr_time;
  }
  std::cout << "avg time 40 threads: " << all_time / 10 << std::endl;
  std::cout << "S40: " << one_thread_time / all_time << std::endl << std::endl;
  return 0;
}