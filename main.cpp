#include "src/dag_engine.h"
#include "src/event_loop.h"
#include "src/worker_pool.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

class MyContext : public Context {
public:
  int result;
  std::vector<std::string> path;
};
class A : public Node {
public:
  A() : Node("A") {}
  virtual NodeProcessStatus Process(std::shared_ptr<Context> ctx,
                                    std::function<void()> finish_func) {
    std::shared_ptr<MyContext> mctx = std::dynamic_pointer_cast<MyContext>(ctx);
    mctx->result++;
    mctx->path.push_back("A");
    finish_func();
    return NodeProcessStatus::OK;
  }
};
class B : public Node {
public:
  B() : Node("B") {}
  virtual NodeProcessStatus Process(std::shared_ptr<Context> ctx,
                                    std::function<void()> finish_func) {
    std::shared_ptr<MyContext> mctx = std::dynamic_pointer_cast<MyContext>(ctx);

    mctx->result++;
    mctx->path.push_back("B");
    finish_func();
    return NodeProcessStatus::OK;
  }
};
class C : public Node {
public:
  C() : Node("C") {}
  virtual NodeProcessStatus Process(std::shared_ptr<Context> ctx,
                                    std::function<void()> finish_func) {
    std::shared_ptr<MyContext> mctx = std::dynamic_pointer_cast<MyContext>(ctx);

    mctx->result++;
    mctx->path.push_back("C");
    finish_func();
    return NodeProcessStatus::OK;
  }
};
int main() {
  auto event_loop = EventLoop();
  auto worker = std::make_shared<WorkerPool>(10);
  worker->Run();

  auto dag = std::make_shared<DAGEngine>(worker);
  dag->RegisteNode(std::make_shared<A>());
  dag->RegisteNode(std::make_shared<B>());
  dag->RegisteNode(std::make_shared<C>());
  auto task = std::make_shared<DAGTask>();
  task->ctx_ = std::make_shared<MyContext>();
  task->graph_ = "A->B\nA->C";
  task->end_call_back_ = [](std::shared_ptr<Context> ctx) {
    std::shared_ptr<MyContext> mctx = std::dynamic_pointer_cast<MyContext>(ctx);
    std::cout << "myCtx:" << mctx->result << std::endl;
    for (auto &path : mctx->path) {
      std::cout << path << std::endl;
    }
  };
  auto task_ctx = dag->NewTask(task);
  dag->Run(task_ctx);
  // 所以dag的引擎也应该放在event_loop里？
  // 我理解，每当有个事件完成的时候，应该通知event_loop，然后插入新任务，去执行，不过这个任务似乎需要加锁才行？
  // 执行完了需要notifyAll
  //

  event_loop.Run();
  std::this_thread::sleep_for(std::chrono::seconds(10));
}