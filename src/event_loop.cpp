#include "event_loop.h"
#include <errno.h>
#include <exception>
#include <iostream>
#include <string>
#include <sys/epoll.h>
EventLoop::EventLoop() {
  std::cout << "init" << std::endl;
  int epoll_fd = epoll_create(1);
  if (epoll_fd == -1) {
    throw std::runtime_error("epoll create error" + std::to_string(errno));
  }
  std::cout << epoll_fd << std::endl;
}

EventLoop::~EventLoop() {}

void EventLoop::Run() {}
