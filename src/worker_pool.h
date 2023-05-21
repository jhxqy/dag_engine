#ifndef WORK_POOL_H
#define WORK_POOL_H
#include "blocked_queue.h"
#include <atomic>
#include <functional>
#include <queue>
#include <thread>
#include <vector>
class WorkerPool {
  std::vector<std::thread> threads_;
  std::vector<BlockedQueue<std::function<void()>>> task_queues_;
  int thread_num_;
  std::atomic_bool end_;

public:
  WorkerPool(int thread_num);
  void Work(int idx);
  void AddTask(std::function<void()>);
  void Stop();
  void Run();
};

#endif