mkdir build
cd build
cmake ..
make

rows_values=(20000 40000)
threads_values=(2 4 7 8 16 20 40)

for rows in "${rows_values[@]}"; do
  for threads in "${threads_values[@]}"; do
    for i in {1..10}; do
      echo "Запуск с $rows матрицей и $threads потоками (Запуск номер $i)"
      ./matrix_mul $rows $threads
    done
  done
done
