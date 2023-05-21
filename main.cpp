#include <chrono>
#include <iostream>
#include <thread>
#include "src/event_loop.h"
#include "src/worker_pool.h"
int main(){
    auto event_loop = EventLoop();
    auto worker = WorkerPool(10);
    worker.Run();
    
    worker.AddTask([](){
        
    });
    // 所以dag的引擎也应该放在event_loop里？
    // 我理解，每当有个事件完成的时候，应该通知event_loop，然后插入新任务，去执行，不过这个任务似乎需要加锁才行？
    // 执行完了需要notifyAll
    // 
    
    event_loop.Run();
    std::this_thread::sleep_for(std::chrono::seconds(10));
}