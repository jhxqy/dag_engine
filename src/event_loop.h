#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H
#include <iostream>
#include <functional>
class EventLoop{
private:

public:
    EventLoop();
    void Run();
    /*
    1. 首先epoll的功能在于，在一个任务上，等待任务可以执行，任务可执行的触发条件是fd可读或可写
       // 所以每个任务需要对应一个fd，或者从fd里读到的key可以支持调度到多个task，比如每次读一个整型，代表任务的key
       // 每个连接确实需要一个fd
       //  所以这个接口首先有一个类型，但是当我的epoll挂起的时候，我如何去添加新的任务呢
       // 
    */
    // 分为几种，一个是一个固定套接字，告诉我cpu密集型ready了，似乎可以复用
    //    我怎么知道是哪个task可以执行了？给每个task一个key吗，
    // 一个是新的fd，在我这里等待可读或可写
    // 一个是listen 的fd，在我这里等待可读。
    void AddIOEvent(int fd,std::function<void(int)> callback);
    /*
    是不是如果是cpu task的话，我只需要把这个任务送回去就好了，告诉他是哪个任务可以执行了
    */
    void AddCPUEvent(int callbackID,std::function<void()>);

    void NotifyCPUEvent(int callbackfd); // 在其他线程执行
    ~EventLoop();
};




#endif