#include <cstdlib>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>
#include <chrono>
#include <math.h>
#define NUM_THREADS 40


double func(double x)
{
  return exp(-x * x); 
}


double integrate_omp(double (*func)(double), double a, double b, int n)
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


const double PI = 3.14159265358979323846;
const double a = -4.0;
const double b = 4.0;
const int nsteps = 40000000;

double run_parallel()
{
  const auto start{std::chrono::steady_clock::now()};
  double res = integrate_omp(func, a, b, nsteps);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};

  printf("Result (parallel): %.12f; error %.12f\n", res, fabs(res - sqrt(PI)));
  return elapsed_seconds.count();
}


double run_serial()
{
  const auto start{std::chrono::steady_clock::now()};
  double res = integrate(func, a, b, nsteps);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};

  printf("Result (serial): %.12f; error %.12f\n", res, fabs(res - sqrt(PI)));
  return elapsed_seconds.count();
}


int main(int argc, char **argv)
{
  printf("Integration f(x) on [%.12f, %.12f], nsteps = %d\n", a, b, nsteps);
  double tserial = run_serial();
  double tparallel = run_parallel();
  printf("Execution time (serial): %.6f\n", tserial);
  printf("Execution time (parallel): %.6f\n", tparallel);
  printf("Speedup: %.2f\n", tserial / tparallel);
  return 0;
}