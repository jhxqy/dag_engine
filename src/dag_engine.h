#ifndef DAG_ENGINE_H
#define DAG_ENGINE_H
#include "worker_pool.h"
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
struct Context : public std::enable_shared_from_this<Context> {
  virtual ~Context() {}
};

enum class NodeProcessStatus { WAIT, OK };
class Node {
  std::string name_;

public:
  Node(const std::string &name) : name_(name) {}
  virtual std::string GetNodeName() { return this->name_; }
  virtual NodeProcessStatus Process(std::shared_ptr<Context> ctx,
                                    std::function<void()> finish_func) {
    finish_func();
    return NodeProcessStatus::OK;
  }
  NodeProcessStatus NodeProcess(std::shared_ptr<Context> ctx,
                                std::function<void()> finish_func) {
    return this->Process(ctx, finish_func);
  }
};
struct NodeContext {
  std::shared_ptr<Node> node_;
  std::atomic_int in_degree_;
  std::unordered_set<std::shared_ptr<NodeContext>> downstream_nodes_;
  NodeContext(std::shared_ptr<Node> node) : node_(node), in_degree_(0) {}
};

class DAGEngine;
struct DAGTask;
class DAGContext : public std::enable_shared_from_this<DAGContext> {
  std::shared_ptr<DAGEngine> engine_;
  std::shared_ptr<DAGTask> task_;
  std::mutex m_;
  std::unordered_map<std::string, std::shared_ptr<NodeContext>> node_ctx_map_;
  std::atomic_int remain_task_num_;
  // 入度为0的
public:
  std::unordered_set<std::shared_ptr<NodeContext>> GetRunableNodes();
  DAGContext(std::shared_ptr<DAGEngine> engine, std::shared_ptr<DAGTask> task)
      : engine_(engine), task_(task), remain_task_num_(0) {}
  int AddDeps(const std::string &from, const std::string &to);
  void Finish(const std::string &name);
  std::shared_ptr<DAGTask> GetTask() { return task_; }
};

// 直觉上这个需要跑在epoll线程上，不知道有没有什么问题。
// 在worker线程上处理这个任务似乎会冲突，不过如果context特别多怎么办？加锁？

struct DAGTask {
  std::string graph_;
  std::function<void(std::shared_ptr<Context>)> end_call_back_;
  std::shared_ptr<Context> ctx_;
};

class DAGEngine : public std::enable_shared_from_this<DAGEngine> {
  std::unordered_map<std::string, std::shared_ptr<Node>> register_nodes_;
  std::shared_ptr<WorkerPool> workerPool_;

public:
  DAGEngine(std::shared_ptr<WorkerPool> work_pool) : workerPool_(work_pool) {}
  int RegisteNode(std::shared_ptr<Node> node);
  std::shared_ptr<Node> GetNodeFromName(const std::string &name) {
    auto iter = register_nodes_.find(name);
    if (iter == register_nodes_.end()) {
      return nullptr;
    }
    return iter->second;
  }
  // 至于说在哪run，这是一个问题
  std::shared_ptr<DAGContext> NewTask(std::shared_ptr<DAGTask> task);
  int Run(
      std::shared_ptr<DAGContext>
          ctx); // 需要找出当前有多少个节点可以执行，然后把它放进work_threads里去
  void RunInWorker(std::function<void()> task);
};

#endif