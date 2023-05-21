#include "dag_engine.h"
#include <algorithm>
#include <memory>
#include "util.h"
#include <exception>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

int DAGContext::AddDeps(const std::string&from, const std::string&to){
    auto from_iter = node_ctx_map_.find(from);
    if(from_iter == node_ctx_map_.end()){
        auto from_node = engine_->GetNodeFromName(from);
        if(from_node == nullptr){
            return -1;
        }
        node_ctx_map_[from] = std::make_shared<NodeContext>(from_node);
    }
    if(from == to){
        return 0;
    }
    auto to_iter = node_ctx_map_.find(to);
    if(to_iter == node_ctx_map_.end()){
        auto to_node = engine_->GetNodeFromName(to);
        if(to_node == nullptr){
            return -1;
        }
        node_ctx_map_[to] = std::make_shared<NodeContext>(to_node);
    }
    node_ctx_map_[to]->in_degree_++;
    node_ctx_map_[from]->downstream_nodes_.insert(node_ctx_map_[to]);
    return 0;
}

std::unordered_set<std::shared_ptr<NodeContext>> DAGContext::GetRunableNodes(){
    std::lock_guard<std::mutex> lg(m_);
    return std::move(this->runable_nodes_);
}
void DAGContext::Finish(const std::string &node_name){
    auto node_ctx_ptr = node_ctx_map_[node_name];
    std::unordered_set<std::shared_ptr<NodeContext>> runable_nodes;
    for(auto downstream_node:node_ctx_ptr->downstream_nodes_){
        auto in_degree = downstream_node->in_degree_.fetch_sub(1, std::memory_order_seq_cst) - 1;
        if(in_degree == 0){
            runable_nodes.insert(downstream_node);
        }
    }
    std::lock_guard<std::mutex> lg(m_);
    runable_nodes_.insert(runable_nodes.begin(),runable_nodes.end());
    // 插入run任务，进行循环，如果全部执行完了，就结束
    // 直接在这里触发下游任务吖
    if(runable_nodes.size() == 0){
        return;
    }
    
}

/*
A->B
A->C
A->D
*/
int DAGEngine::RegisteNode(std::shared_ptr<Node> node){
    register_nodes_[node->GetNodeName()] = node;
    return 0;
}

std::shared_ptr<DAGContext> DAGEngine::LoadGraph(const std::string&graph){
    // 逐行读取
    auto lines = SplitString(graph,"\n");
    if (lines.size() == 0) {
        throw std::runtime_error("graph is null");
    }
    auto dagCtxPtr = std::make_shared<DAGContext>(shared_from_this());
    for(auto &line :lines){
        auto tokens = SplitString(line, "->");
        for(int i = 0; i < tokens.size()-1; i++){
            dagCtxPtr->AddDeps(tokens[i], tokens[i+1]);
        }
    }
    return dagCtxPtr;
}


int DAGEngine::Run(std::shared_ptr<DAGContext> ctx){
    auto runable_nodes = ctx->GetRunableNodes();

    return 0;
}