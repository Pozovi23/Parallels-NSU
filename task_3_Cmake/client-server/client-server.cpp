#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <functional>
#include <fstream>
#include <vector>
#include <random>
#include <unordered_map>


template <typename T>
class Server {
public:
  Server() {}

  void start() {
    server = std::thread([this] {
      while (true) {
        TaskStruct task_struct;
        {
          std::unique_lock<std::mutex> lock(mtx);
          cv.wait(lock, [this] { return !tasks.empty() || !running; });

          if (!running && tasks.empty()) break;

          task_struct = std::move(tasks.front());
          tasks.pop();
        }
        task_struct.task(); 
      }
    });
  }

  int add_task(std::function<T()> func) {
    std::unique_lock<std::mutex> lock(mtx);
    int curr_id = id_counter;
    id_counter++;
    std::packaged_task<T()> task(func);
    std::future<T> result = task.get_future();
    future_tasks[curr_id] = std::move(result);
    tasks.push({curr_id, std::move(task)});
    cv.notify_one();
    return curr_id;
  }

  T request_result(int id_res) {
    std::future<T> future_task;
    {
      std::unique_lock<std::mutex> lock(mtx);
      future_task = std::move(future_tasks.at(id_res));
      future_tasks.erase(id_res);
    }
    return future_task.get();
  }

  void stop() {
    {
      std::unique_lock<std::mutex> lock(mtx);
      running = false;
    }
    cv.notify_one();
    if (server.joinable())
      server.join();
  }

private:
  struct TaskStruct {
    int id;
    std::packaged_task<T()> task;
  };

  std::thread server;
  std::unordered_map<int, std::future<T>> future_tasks;
  std::mutex mtx;
  std::condition_variable cv;
  bool running = true;
  int id_counter = 0;
  std::queue<TaskStruct> tasks;
};


void client_sin(Server<double>& server, int tasks_count) {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(-100, 100);
  std::ofstream output("../client_sin.txt");
  for (int i = 0; i < tasks_count; ++i) {
    double arg = dist(gen);
    int task_id = server.add_task([arg]() {
      return std::sin(arg);
    });
    double result = server.request_result(task_id);
    output << "sin(" << arg << ") = " << result << "\n";
  }
  output.close();
}


// void client_sqrt(Server<double>& server, int tasks_count) {
//   std::mt19937 gen(std::random_device{}());
//   std::uniform_real_distribution<double> dist(0.0, 1000.0);
//   std::ofstream output("../client_sqrt.txt");
//   for (int i = 0; i < tasks_count; ++i) {
//     double arg = dist(gen);
//     auto done_task = server.add_task([arg]() {
//       return std::sqrt(arg);
//     });
//     output << "sqrt(" << arg << ") = " << done_task << "\n";
//   }
//   output.close();
// }


// void client_pow(Server<double>& server, int tasks_count) {
//   std::mt19937 gen(std::random_device{}());
//   std::uniform_real_distribution<double> bs(1.0, 10.0);
//   std::uniform_real_distribution<double> expp(1.0, 5.0);
//   std::ofstream output("../client_pow.txt");
//   for (int i = 0; i < tasks_count; ++i) {
//     double base = bs(gen);
//     double exp = expp(gen);
//     auto done_task = server.add_task([base, exp]() {
//       return std::pow(base, exp);
//     });
//     output << base << "^" << exp << " = " << done_task << "\n";
//   }
//   output.close();
// }

int main() {
  Server<double> server;
  server.start();

  const int N = 10000;

  std::thread t1(client_sin, std::ref(server), N);
  // std::thread t2(client_sqrt, std::ref(server), N);
  // std::thread t3(client_pow, std::ref(server), N);

  t1.join();
  // t2.join();
  // t3.join();

  server.stop();
}