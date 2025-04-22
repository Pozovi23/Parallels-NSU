#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <boost/program_options.hpp>

#define OUT_FILE "result.dat"


int NX = 20;
int NY = 20;
double EPS = 0.000001;
int ITER = 1000000;
#pragma acc declare create(NX, NY, EPS, ITER)

int SIZE = NX * NY;
#pragma acc declare create(SIZE)

#define TAU -0.01


double get_a(int row, int col) {
	if (row==col) return -4;
	if (row+1==col) return 1;
	if (row-1==col) return 1;
	if (row+NX==col) return 1;
	if (row-NX==col) return 1;
	return 0;
}


void init_matrix(double *A) {
	#pragma acc parallel loop collapse(2) present(A[0:SIZE*SIZE])
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			A[i * SIZE + j] = get_a(i, j);
		}
	}
}


void init_b(double *b) {
	double corners[4] = {10.0, 20.0, 30.0, 20.0};
	
	#pragma acc parallel loop present(b[0:SIZE])
	for (int i = 0; i < SIZE; i++) {
		b[i] = 0.0;
	}
	
	b[0] = corners[0];
	b[NX-1] = corners[1];
	b[NX*NY-1] = corners[2];
	b[NX*(NY-1)] = corners[3];
	
	#pragma acc update device(b[0], b[NX-1], b[NX*NY-1], b[NX*(NY-1)])
	
	#pragma acc parallel loop present(b[0:SIZE])
	for (int i = 1; i < NX-1; i++) {
		double coef = (double)i / (NX-1);
		b[i] = b[0] * (1.0 - coef) + b[NX-1] * coef;
	}
	
	#pragma acc parallel loop present(b[0:SIZE])
	for (int i = 1; i < NY-1; i++) {
		double coef = (double)i / (NY-1);
		b[i*NX + NX-1] = b[NX-1] * (1.0 - coef) + b[NX*NY-1] * coef;
	}
	
	#pragma acc parallel loop present(b[0:SIZE])
	for (int i = 1; i < NX-1; i++) {
		double coef = (double)i / (NX-1);
		b[NX*(NY-1) + i] = b[NX*(NY-1)] * (1.0 - coef) + b[NX*NY-1] * coef;
	}
	
	#pragma acc parallel loop present(b[0:SIZE])
	for (int i = 1; i < NY-1; i++) {
		double coef = (double)i / (NY-1);
		b[i*NX] = b[0] * (1.0 - coef) + b[NX*(NY-1)] * coef;
	}
}


double norm(double *x) {
	double result = 0.0;
	#pragma acc parallel loop reduction(+:result) present(x[0:SIZE])
	for (int i = 0; i < SIZE; i++) {
		result += x[i] * x[i];
	}
	return sqrt(result);
}


void solve_simple_iter(double *A, double *x, double *b) {
	double norm_b = norm(b);
	double norm_Axmb;
	double *Axmb = new double[SIZE];
	
	#pragma acc enter data create(Axmb[0:SIZE])
	
	int iter = 0;
	do {
		#pragma acc parallel loop present(A, x, b, Axmb)
		for (int i = 0; i < SIZE; i++) {
			Axmb[i] = -b[i];
			#pragma acc loop seq
			for (int j = 0; j < SIZE; j++) {
				Axmb[i] += A[i*SIZE + j] * x[j];
			}
		}

		norm_Axmb = norm(Axmb);

		#pragma acc parallel loop present(x, Axmb)
		for (int i = 0; i < SIZE; i++) {
			x[i] -= TAU * Axmb[i];
		}

		iter++;
		printf("iter: %d, %lf >= %lf\r", iter, norm_Axmb/norm_b, EPS);
		fflush(stdout);
	} while (norm_Axmb/norm_b >= EPS && iter < ITER);
	
	printf("\nFinal iteration: %d, error: %le\n", iter, norm_Axmb/norm_b);
	
	#pragma acc exit data delete(Axmb[0:SIZE])
	delete[] Axmb;
}

int main(int argc, char *argv[]) {
	boost::program_options::options_description desc("Options");
	desc.add_options()
		("nx", boost::program_options::value<int>(), "Matrix size in X")
		("ny", boost::program_options::value<int>(), "Matrix size in Y")
		("eps", boost::program_options::value<double>(), "Precision")
		("iters", boost::program_options::value<int>(), "Max iterations");
	
	boost::program_options::variables_map vm;

	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("nx")) NX = vm["nx"].as<int>();
	if (vm.count("ny")) NY = vm["ny"].as<int>();
	if (vm.count("eps")) EPS = vm["eps"].as<double>();
	if (vm.count("iters")) ITER = vm["iters"].as<int>();

	SIZE = NX * NY;
	#pragma acc update device(SIZE, NX, NY, EPS, ITER)

	double *A = new double[SIZE*SIZE];
	double *b = new double[SIZE];
	double *x = new double[SIZE];
	
	#pragma acc enter data create(A[0:SIZE*SIZE], b[0:SIZE], x[0:SIZE])
	
	init_matrix(A);
	init_b(b);
	memset(x, 0, sizeof(double)*SIZE);
	
	solve_simple_iter(A, x, b);
	
	FILE *f = fopen(OUT_FILE, "wb");
	#pragma acc update self(x[0:SIZE])
	fwrite(x, sizeof(double), SIZE, f);
	fclose(f);
	
	#pragma acc exit data delete(A[0:SIZE*SIZE], b[0:SIZE], x[0:SIZE])
	delete[] A;
	delete[] b;
	delete[] x;
	
	return 0;
}