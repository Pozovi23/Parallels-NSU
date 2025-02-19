#include <iostream>
#include <cstdlib>
#include <cmath>
#include <chrono>

double E = std::log(0.00001);


void multiply_matrix_and_vector(double* A, double* x, double* res, int size_mas) {
  for (int i = 0; i < size_mas; i++) {
    res[i] = 0.0;
    for (int j = 0; j < size_mas; j++) {
      res[i] += A[i * size_mas + j] * x[j];
    }
  }
}


void sub(double* x, double* y, int size_mas) {
  for (int i = 0; i < size_mas; i++) {
    x[i] -= y[i];
  }
}


void mul_by_const(double* x, double CONST, int size_mas) {
  for (int i = 0; i < size_mas; i++) {
    x[i] *= CONST;
  }
}


double L2_norm(double* vector, int size_mas) {
  double sum = 0;
  for (int i = 0; i < size_mas; i++) {
    sum += vector[i];
  }

  return sqrt(sum);
}


bool is_continuing_needed(double* vector_for_criteria, double* b, int size_mas) {
  double result = std::log(L2_norm(vector_for_criteria, size_mas)) - std::log(L2_norm(b, size_mas));
  std::cout << std::fixed;

  std::cout.precision(8);

  
  std::cout << result << std::endl;
  if (result <= E) {
    return false;
  } else {
    return true;
  }
}


void solving(double* A, double* x_previous, double* x_new, double *b, int size_mas, double CONST) {
  double* middle_res = (double*)malloc(sizeof(double) * size_mas);
  int i = 0;
  while (1) {
    multiply_matrix_and_vector(A, x_previous, middle_res, size_mas);
    sub(middle_res, b, size_mas);
    if (!is_continuing_needed(middle_res, b, size_mas)) {
      return;
    }
    mul_by_const(middle_res, CONST, size_mas);
    sub(x_previous, middle_res, size_mas);
    i++;
    if (i == 100) break;
  }
}


int main() {
  double *A, *x_previous, *x_new, *b;
  int size_mas = 10;

  A = (double*)malloc(sizeof(double) * size_mas * size_mas);
  b = (double*)malloc(sizeof(double) * size_mas);
  x_previous = (double*)malloc(sizeof(double) * size_mas);
  x_new = (double*)malloc(sizeof(double) * size_mas);

  for (int i = 0; i < size_mas; i++) {
    for (int j = 0; j < size_mas; j++) {
      if (i == j) {
        A[i * size_mas + j] = 2.0;
      } else {
        A[i * size_mas + j] = 1.0;
      }
    }

    b[i] = size_mas + 1;
    x_previous[i] = 0.0;
  }
  std::cout << "Start" << std::endl;
  const auto start{std::chrono::steady_clock::now()};
  solving(A, x_previous, x_new, b, size_mas, 0.01);
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count();

  
  for (int i = 0; i < size_mas; i++) {
    std::cout << x_previous[i] << std::endl;
  }
  return 0;
}