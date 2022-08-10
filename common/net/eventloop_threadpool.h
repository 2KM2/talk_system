#pragma once

#include "noncopyable.h"

#include <functional>
#include <string>
#include <vector>
#include <memory>

class CEventLoop;
class CEventLoopThread;

class CEventLoopThreadPool:noncopyable
{
public:
    using ThreadInitCallback = std::function<void(CEventLoop *)>;
    CEventLoopThreadPool(CEventLoop *baseLoop, const std::string &nameArg);
    ~CEventLoopThreadPool();
    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start(const ThreadInitCallback &cb = ThreadInitCallback());
    // 如果工作在多线程中，baseLoop_默认以轮询的方式分配channel给subloop
    CEventLoop* getNextLoop();

    std::vector<CEventLoop*> getAllLoops();

    bool started() const { return started_; }
    const std::string name() const { return name_; }
private:
    CEventLoop *baseLoop_;// 用户创建的loop,传进来的
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<CEventLoopThread>> threads_;//subloop one loop per thread
    std::vector<CEventLoop*> loops_;//所有eventloop指针
};
