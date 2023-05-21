#ifndef WORK_POOL_H
#define WORK_POOL_H
#include <atomic>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include "blocked_queue.h"
class WorkerPool{
    std::vector<std::thread> threads_;
    std::vector<BlockedQueue<std::function<void()> >> task_queues_;
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