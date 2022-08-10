#pragma once
#include "noncopyable.h"
#include  "thread.h"


#include <functional>
#include <mutex>
#include <condition_variable>
#include <string>


class CEventLoop;

class CEventLoopThread:noncopyable
{
public:
    using ThreadInitCallback = std::function<void(CEventLoop *loop)>;
    CEventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::string &name = std::string());
    ~CEventLoopThread();

    CEventLoop* startLoop();
private:
    void threadFunc();
    CEventLoop *loop_;
    bool exiting_;
    CThread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
};