#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <boost/program_options.hpp>
#include <chrono>

#define OUT_FILE "result.dat"


int NX = 20;
int NY = 20;
double EPS = 0.000001;
int ITER = 1000000;

int SIZE = NX * NY;


#define TAU -0.01


void init_b(double *b) {
	double corners[4] = {10.0, 20.0, 30.0, 20.0};
	
	#pragma acc parallel loop independent
	for (int i = 0; i < SIZE; i++) {
		b[i] = 0.0;
	}
	
	b[0] = corners[0];
	b[NX-1] = corners[1];
	b[NX*NY-1] = corners[2];
	b[NX*(NY-1)] = corners[3];
	
	
	#pragma acc parallel loop independent
	for (int i = 1; i < NX-1; i++) {
		double coef = (double)i / (NX-1);
		b[i] = b[0] * (1.0 - coef) + b[NX-1] * coef;
	}
	
	#pragma acc parallel loop independent
	for (int i = 1; i < NY-1; i++) {
		double coef = (double)i / (NY-1);
		b[i*NX + NX-1] = b[NX-1] * (1.0 - coef) + b[NX*NY-1] * coef;
	}
	
	#pragma acc parallel loop independent
	for (int i = 1; i < NX-1; i++) {
		double coef = (double)i / (NX-1);
		b[NX*(NY-1) + i] = b[NX*(NY-1)] * (1.0 - coef) + b[NX*NY-1] * coef;
	}
	
	#pragma acc parallel loop independent
	for (int i = 1; i < NY-1; i++) {
		double coef = (double)i / (NY-1);
		b[i*NX] = b[0] * (1.0 - coef) + b[NX*(NY-1)] * coef;
	}
}


double norm(double *x) {
	double result = 0.0;
	#pragma acc parallel loop reduction(+:result)
	for (int i = 0; i < SIZE; i++) {
		result += x[i] * x[i];
	}
	return sqrt(result);
}


void solve_simple_iter(double *A, double *x, double *b) {
	double norm_b = norm(b);
	double norm_Axmb;
	double *Axmb = new double[SIZE];
	
	
	int iter = 0;
	do {
		#pragma acc parallel loop 
		for (int i = 0; i < SIZE; i++) {
			Axmb[i] = -b[i];
			#pragma acc loop seq
			for (int j = 0; j < SIZE; j++) {
				Axmb[i] += A[i*SIZE + j] * x[j];
			}
		}

		norm_Axmb = norm(Axmb);

		#pragma acc parallel loop independent
		for (int i = 0; i < SIZE; i++) {
			x[i] -= TAU * Axmb[i];
		}

		iter++;
		if (iter % 200 == 0) {
			printf("iter: %d, %lf >= %lf\r", iter, norm_Axmb/norm_b, EPS);
			fflush(stdout);
		}
	} while (norm_Axmb/norm_b >= EPS && iter < ITER);
	
	printf("\niterations reached: %d, error reaached: %lf\n", iter, norm_Axmb/norm_b);
	
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
	double *b = new double[SIZE];
	double *x = new double[SIZE];
	
	init_b(b);
	memset(x, 0, sizeof(double)*SIZE);

	const auto start{std::chrono::steady_clock::now()};
	solve_simple_iter(A, x, b);
	const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << elapsed_seconds.count() << std::endl << std::endl;

	FILE *f = fopen(OUT_FILE, "wb");
	fwrite(x, sizeof(double), SIZE, f);
	fclose(f);
	
	delete[] A;
	delete[] b;
	delete[] x;
	
	return 0;
}