#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <boost/program_options.hpp>

#define OUT_FILE "result.dat"

int NX = 13;
int NY = 13;
double EPS = 1e-6;
int MAX_ITER = 1e6;

__global__ void count_matrix(double* src, double* dst, int width, int height) {
  int i = blockIdx.y * blockDim.y + threadIdx.y + 1;
  int j = blockIdx.x * blockDim.x + threadIdx.x + 1;
  
  if (i < height - 1 && j < width - 1) {
    dst[i * width + j] = 0.25 * (src[i * width + j - 1] + 
      src[i * width + j + 1] + 
      src[(i - 1) * width + j] + 
      src[(i + 1) * width + j]);
  }
}

__global__ void square_kernel(double* matrix, double* result, int size) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < size) {
    result[idx] = matrix[idx] * matrix[idx];
  }
}

void init_matrix(double* matrix) {
  double corners[4] = {10.0, 20.0, 30.0, 20.0};
  
  for (int i = 0; i < (NX + 2) * (NY + 2); i++) {
    matrix[i] = 0.0;
  }
  
  matrix[(NX + 2) + 1] = corners[0];
  matrix[(NX + 2) * 2 - 2] = corners[1];
  matrix[(NX + 2) * (NY + 1) - 2] = corners[2];
  matrix[(NX + 2) * NY + 1] = corners[3];
  
  for (int i = (NX + 2) + 2, j = 1; i < (NX + 2) * 2 - 2; i++, j++) {
    double coef = (double)(j) / (NX - 1);
    matrix[i] = corners[0] * (1.0 - coef) + corners[1] * coef;
    matrix[(NX + 2) * NY + 1 + j] = corners[3] * (1.0 - coef) + corners[2] * coef;
  }

  for (int i = (NX + 2) * 2 - 2 + (NX + 2), j = 1; i < (NX + 2) * (NY + 1) - 2; i+=(NX + 2), j++) {
    double coef = (double)(j) / (NY - 1);
    matrix[i] = corners[1] * (1.0 - coef) + corners[2] * coef;
    matrix[i - NX + 1] = corners[0] * (1.0 - coef) + corners[3] * coef;
  }
}

double compute_l2_norm(double* d_matrix, int size) {
  double* d_squares;
  cudaMalloc(&d_squares, size * sizeof(double));
  
  dim3 blockDim(256);
  dim3 gridDim((size + blockDim.x - 1) / blockDim.x);
  square_kernel<<<gridDim, blockDim>>>(d_matrix, d_squares, size);
  cudaDeviceSynchronize();
  
  void* d_temp_storage = NULL;
  size_t temp_storage_bytes = 0;
  double sum;
  double* d_sum;
  cudaMalloc(&d_sum, sizeof(double));
  
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, d_squares, d_sum, size);
  cudaMalloc(&d_temp_storage, temp_storage_bytes);
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, d_squares, d_sum, size);
  cudaMemcpy(&sum, d_sum, sizeof(double), cudaMemcpyDeviceToHost);
  
  cudaFree(d_temp_storage);
  cudaFree(d_sum);
  cudaFree(d_squares);
  
  return sqrt(sum);
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
  if (vm.count("iters")) MAX_ITER = vm["iters"].as<int>();

  const int width = NX + 2;
  const int height = NY + 2;
  const size_t matrix_size = width * height * sizeof(double);

  double* h_matrix1 = new double[width * height];
  double* h_matrix2 = new double[width * height];
  
  init_matrix(h_matrix1);
  init_matrix(h_matrix2);

  double *d_matrix1, *d_matrix2;
  cudaMalloc(&d_matrix1, matrix_size);
  cudaMalloc(&d_matrix2, matrix_size);

  cudaMemcpy(d_matrix1, h_matrix1, matrix_size, cudaMemcpyHostToDevice);
  cudaMemcpy(d_matrix2, h_matrix2, matrix_size, cudaMemcpyHostToDevice);

  double norm_orig = compute_l2_norm(d_matrix1, width * height);

  dim3 block_size(20, 20);
  dim3 grid_size((width + block_size.x - 1) / block_size.x, (height + block_size.y - 1) / block_size.y);

  int iter = 0;
  double error = 1.0;
  bool use_matrix1 = true;

  cudaGraph_t graph;
  cudaGraphExec_t graphExec;

  auto start = std::chrono::steady_clock::now();

  cudaStream_t stream;
  cudaStreamCreate(&stream);
  cudaStreamBeginCapture(stream, cudaStreamCaptureModeGlobal);
  
  for (int i = 0; i < 2000; i++) {
    if (use_matrix1) {
      count_matrix<<<grid_size, block_size, 0, stream>>>(d_matrix1, d_matrix2, width, height);
    } else {
      count_matrix<<<grid_size, block_size, 0, stream>>>(d_matrix2, d_matrix1, width, height);
    }
    use_matrix1 = !use_matrix1;
  }

  cudaStreamEndCapture(stream, &graph);
  cudaGraphInstantiate(&graphExec, graph, NULL, NULL, 0);
  cudaStreamDestroy(stream);

  while (error > EPS && iter < MAX_ITER) {
    cudaGraphLaunch(graphExec, 0);
    cudaDeviceSynchronize();
    
    iter += 2000;
    
    double current_l2_norm = compute_l2_norm(use_matrix1 ? d_matrix1 : d_matrix2, width * height);
    error = current_l2_norm / norm_orig;
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::cout << iter << " " << error << std::endl;
  std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;

  cudaGraphExecDestroy(graphExec);
  cudaGraphDestroy(graph);

  double* result_matrix = use_matrix1 ? h_matrix1 : h_matrix2;
  cudaMemcpy(result_matrix, use_matrix1 ? d_matrix1 : d_matrix2, 
    matrix_size, cudaMemcpyDeviceToHost);

  FILE* f = fopen(OUT_FILE, "wb");
  for (int i = 1; i < height - 1; i++) {
    for (int j = 1; j < width - 1; j++) {
      double value = result_matrix[i * width + j];
      fwrite(&value, sizeof(double), 1, f);
    }
  }
  fclose(f);
  delete[] h_matrix1;
  delete[] h_matrix2;
  cudaFree(d_matrix1);
  cudaFree(d_matrix2);

  return 0;
}