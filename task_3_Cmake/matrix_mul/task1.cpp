#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream>

int rows, columns, num_threads;

void init_vec(double* vec) {
  int chunk_size = rows / num_threads;
  std::vector<std::thread> threads;

  auto lambda = [vec](int start, int end) {
    for (int i = start; i < end; i++) {
      vec[i] = 1;
    } 
  };

  for (int i = 0; i < num_threads; i++) {
    int start = i * chunk_size;
    int end = (i == num_threads - 1) ? rows : (i + 1) * chunk_size;
    threads.emplace_back(lambda, start, end);
  }

  for (auto &thread : threads) {
    thread.join();
  }
}


void init_matrix(double* matrix) {
  int chunk_size = rows / num_threads;
  std::vector<std::thread> threads;

  auto lambda = [matrix](int start, int end) {
    for (int i = start; i < end; i++) {
      for (int j = 0; j < rows; j++) {
        matrix[i * rows + j] = i + j;
      }
    } 
  };

  for (int i = 0; i < num_threads; i++) {
    int start = i * chunk_size;
    int end = (i == num_threads - 1) ? rows : (i + 1) * chunk_size;
    threads.emplace_back(lambda, start, end);
  }

  for (auto &thread : threads) {
    thread.join();
  }
}


int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Мало аргументов" << std::endl;
    return 1;
  }
  rows = columns = std::stoi(argv[1]);
  num_threads = std::stoi(argv[2]);
  double* matrix = new double[rows * columns]; 
  double* vec = new double[rows]; 
  double* res = new double[rows];
  init_vec(vec);
  init_matrix(matrix);

  std::ofstream output_file("../output_matrix.txt", std::ios::app);
  if (!output_file) {
    std::cerr << "Не удалось открыть файл для записи." << std::endl;
    return 1;
  }
  
  output_file << "Started with matrix size: " << columns << " , threads count: " << num_threads << std::endl;
  const auto start{std::chrono::steady_clock::now()};

  std::vector<std::thread> threads;
  int chunck_size = rows / num_threads;

  auto lambda = [matrix, vec, res](int start, int end, int rows) {
    for (int i = start; i < end; i++) {
      for (int j = 0; j < rows; ++j) {
        res[i] += vec[j] * matrix[i * rows + j];
      }
    } 
  };

  for (int i = 0; i < num_threads; i++) {
    int start = i * chunck_size, end = (i == num_threads - 1) ? rows : (i + 1) * chunck_size ;
    threads.emplace_back(lambda, start, end, rows);
  }

  for (auto &thread : threads) {
    thread.join();
  }


  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  output_file << elapsed_seconds.count() << std::endl << std::endl;

  output_file.close();

  delete[] matrix;
  delete[] vec;
  delete[] res;
  return 0;
}