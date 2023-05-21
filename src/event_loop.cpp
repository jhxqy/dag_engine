#include "event_loop.h"
#include <iostream>
#include <sys/epoll.h>
#include <exception>
#include <errno.h>
#include <string>
EventLoop::EventLoop(){
    std::cout<<"init"<<std::endl;
    int epoll_fd = epoll_create(1);
    if(epoll_fd == -1){
        throw std::runtime_error("epoll create error" + std::to_string(errno) );
    }
    std::cout<<epoll_fd<<std::endl;
}

EventLoop::~EventLoop(){

}

void EventLoop::Run(){

    std::cout<<"run"<<std::endl;
}

