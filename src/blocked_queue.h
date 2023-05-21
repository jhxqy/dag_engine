#ifndef BLOCKEND_QUEUE_H
#define BLOCKEND_QUEUE_H

#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
template<typename T>
class BlockedQueue{

    std::mutex m_;
    std::condition_variable cond_;
    std::queue<T> q_;

public:
    
    void Push(const T&v){
        std::lock_guard<std::mutex> lg(m_);
        q_.push(v);
        cond_.notify_all();
    }
    T Pop(){
        std::unique_lock<std::mutex> lk(m_);

        cond_.wait(lk,[this]()->bool{
            return !q_.empty();
        });
        auto value = q_.front();
        q_.pop();
        return value;
    }
    int Size(){
        return q_.size();
    }
};


#endif