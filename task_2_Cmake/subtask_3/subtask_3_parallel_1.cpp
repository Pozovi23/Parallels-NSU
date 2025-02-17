#include <iostream>
#include <cstdlib>
#include <cmath>

double E = 0.00001;


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
  double result = L2_norm(vector_for_criteria, size_mas) / L2_norm(b, size_mas);
  if (result < E) {
    return false;
  } else {
    return true;
  }
}


void solving(double* A, double* x_previous, double* x_new, double *b, int size_mas, double CONST) {
  double* middle_res = (double*)malloc(sizeof(double) * size_mas);;
  while (1) {
    multiply_matrix_and_vector(A, x_previous, middle_res, size_mas);
    sub(middle_res, b, size_mas);
    mul_by_const(middle_res, CONST, size_mas);
    if (!is_continuing_needed(middle_res, b, size_mas)) {
      return;
    }
    sub(x_previous, middle_res, size_mas);
  }
}


int main() {
  double *A, *x_previous, *x_new, *b;
  int size_mas = 500;

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


  solving(A, x_previous, x_new, b, size_mas, 0.005);
  for (int i = 0; i < size_mas; i++) {
    if (x_previous[i] > 0.5 && x_previous[i] < -0.5) {
      std::cout << "!!!!!!!!!!!!!!!";
      break;
    }
  }
  return 0;
}