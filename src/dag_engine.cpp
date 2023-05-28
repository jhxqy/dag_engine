#include "dag_engine.h"
#include "util.h"
#include <algorithm>
#include <atomic>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
int DAGContext::AddDeps(const std::string &from, const std::string &to) {
  auto from_iter = node_ctx_map_.find(from);
  if (from_iter == node_ctx_map_.end()) {
    auto from_node = engine_->GetNodeFromName(from);
    if (from_node == nullptr) {
      return -1;
    }
    node_ctx_map_[from] = std::make_shared<NodeContext>(from_node);
    remain_task_num_.fetch_add(1);
  }
  if (from == to) {
    return 0;
  }
  auto to_iter = node_ctx_map_.find(to);
  if (to_iter == node_ctx_map_.end()) {
    auto to_node = engine_->GetNodeFromName(to);
    if (to_node == nullptr) {
      return -1;
    }
    node_ctx_map_[to] = std::make_shared<NodeContext>(to_node);
    remain_task_num_.fetch_add(1);
  }
  node_ctx_map_[to]->in_degree_++;
  node_ctx_map_[from]->downstream_nodes_.insert(node_ctx_map_[to]);
  return 0;
}
std::unordered_set<std::shared_ptr<NodeContext>> DAGContext::GetRunableNodes() {
  std::unordered_set<std::shared_ptr<NodeContext>> runable_nodes;
  for (auto &node : node_ctx_map_) {
    if (node.second->in_degree_ == 0) {
      runable_nodes.insert(node.second);
    }
  }
  return runable_nodes;
}
void DAGContext::Finish(const std::string &node_name) {
  auto node_ctx_ptr = node_ctx_map_[node_name];
  bool finishd = false;
  int remain_task_num =
      remain_task_num_.fetch_sub(1, std::memory_order_seq_cst) - 1;
  if (remain_task_num == 0) {
    task_->end_call_back_(task_->ctx_);
    return;
  }
  std::unordered_set<std::shared_ptr<NodeContext>> runable_nodes;
  for (auto downstream_node : node_ctx_ptr->downstream_nodes_) {
    auto in_degree =
        downstream_node->in_degree_.fetch_sub(1, std::memory_order_seq_cst) - 1;
    if (in_degree == 0) {
      runable_nodes.insert(downstream_node);
    }
  }
  // 插入run任务，进行循环，如果全部执行完了，就结束
  // 直接在这里触发下游任务吖
  if (runable_nodes.size() == 0) {
    return;
  }
  for (auto node : runable_nodes) {
    auto self = shared_from_this();
    engine_->RunInWorker([self, node]() {
      node->node_->NodeProcess(self->task_->ctx_, [self, node]() {
        self->Finish(node->node_->GetNodeName());
      });
    });
  }
  return;
}

/*
A->B
A->C
A->D
*/
int DAGEngine::RegisteNode(std::shared_ptr<Node> node) {
  register_nodes_[node->GetNodeName()] = node;
  return 0;
}

std::shared_ptr<DAGContext> DAGEngine::NewTask(std::shared_ptr<DAGTask> task) {
  // 逐行读取
  auto lines = SplitString(task->graph_, "\n");
  if (lines.size() == 0) {
    throw std::runtime_error("graph is null");
  }
  auto dagCtxPtr = std::make_shared<DAGContext>(shared_from_this(), task);
  for (auto &line : lines) {
    auto tokens = SplitString(line, "->");
    for (int i = 0; i < tokens.size() - 1; i++) {
      std::cout << tokens[i] << " -> " << tokens[i + 1] << std::endl;
      dagCtxPtr->AddDeps(tokens[i], tokens[i + 1]);
    }
  }
  return dagCtxPtr;
}

int DAGEngine::Run(std::shared_ptr<DAGContext> ctx) {
  // 计算出所有当前可执行的，然后丢给task里
  auto self = shared_from_this();
  RunInWorker([self, ctx]() {
    auto runable_nodes = ctx->GetRunableNodes();
    for (auto &node : runable_nodes) {
      self->RunInWorker([self, ctx, node]() {
        node->node_->NodeProcess(ctx->GetTask()->ctx_, [ctx, node]() {
          ctx->Finish(node->node_->GetNodeName());
        });
      });
    }
  });
  return 0;
}

void DAGEngine::RunInWorker(std::function<void()> task) {
  workerPool_->Execute(task);
}