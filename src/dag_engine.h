#ifndef DAG_ENGINE_H
#define DAG_ENGINE_H
#include <atomic>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <queue>
struct Context : public std::enable_shared_from_this<Context>{};

class Node{
std::string name_;
public:
    Node(const std::string &name):name_(name){

    }
    virtual std::string GetNodeName(){
        return this->name_;
    }
    virtual int Process(std::shared_ptr<Context> ctx){
        return 0;
    }
    virtual void PreProcess(std::shared_ptr<Context> ctx){}
    virtual void AfterProecess(std::shared_ptr<Context> ctx){}
    void NodeProcess(std::shared_ptr<Context> ctx){
        PreProcess(ctx);
        this->Process(ctx);
        AfterProecess(ctx);
    }
};
struct NodeContext{
    std::shared_ptr<Node> node_;
    std::atomic_int in_degree_;
    std::unordered_set<std::shared_ptr<NodeContext>> downstream_nodes_;
    NodeContext(std::shared_ptr<Node> node):node_(node),in_degree_(0){}
};

class DAGEngine;

class DAGContext:public std::enable_shared_from_this<DAGContext>{
    std::shared_ptr<DAGEngine> engine_;
    std::mutex m_;
    std::unordered_set<std::shared_ptr<NodeContext>> runable_nodes_;
    std::unordered_map<std::string,std::shared_ptr<NodeContext>> node_ctx_map_;
    
    // 入度为0的
public:
    std::unordered_set<std::shared_ptr<NodeContext>> GetRunableNodes();
    DAGContext(std::shared_ptr<DAGEngine> engine):engine_(engine){
    }
    int AddDeps(const std::string&from,const std::string&to);
    void Finish(const std::string&name);
};



// 直觉上这个需要跑在epoll线程上，不知道有没有什么问题。
// 在worker线程上处理这个任务似乎会冲突，不过如果context特别多怎么办？加锁？
    
class DAGEngine :public 
std::enable_shared_from_this<DAGEngine>{
    std::unordered_map<std::string, std::shared_ptr<Node>> register_nodes_;
public:
    int RegisteNode(std::shared_ptr<Node> node);
    std::shared_ptr<Node> GetNodeFromName(const std::string&name){
        auto iter = register_nodes_.find(name);
        if (iter == register_nodes_.end()){
            return nullptr;
        }
        return iter->second;
    }
    // 至于说在哪run，这是一个问题
    std::shared_ptr<DAGContext> LoadGraph(const std::string&graph++++++++++++++++);
    int Run(std::shared_ptr<DAGContext> ctx); // 需要找出当前有多少个节点可以执行，然后把它放进work_threads里去
};

#endif