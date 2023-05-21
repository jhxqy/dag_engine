#include "worker_pool.h"
#include "blocked_queue.h"
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
WorkerPool::WorkerPool(int thread_num):task_queues_(thread_num) {
    this->thread_num_ = thread_num;
    
}

void WorkerPool::Run(){
    for (int i = 0; i < thread_num_; i++) {
        this->threads_.push_back(std::thread([this](int idx){
            return this->Work(idx);
        },i));
    }
}

void WorkerPool::Work(int idx){
    auto &q = this->task_queues_[idx];
    while(!end_){
        auto task = q.Pop();
        std::cout<<"task"<<std::endl;
        task();
    }
}

void WorkerPool::AddTask(std::function<void()> v){
    int minIdx = 0;
    int minSize = 1000;
    int queueIdx = 0;
    for(int i = 0;i<thread_num_;i++){
        int nowSize = task_queues_[i].Size();
        if(nowSize == 0){
            queueIdx = i;
        }
        if(nowSize<minSize){
            minIdx = queueIdx;
            minSize = nowSize;
        }
    }
    queueIdx = queueIdx==0?minIdx:queueIdx;
    task_queues_[queueIdx].Push(v);
}

void WorkerPool::Stop(){
    end_ = true;
}

