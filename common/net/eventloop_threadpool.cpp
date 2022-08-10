#include "eventloop_threadpool.h"
#include  "eventloop_thread.h"
#include <memory>



CEventLoopThreadPool::CEventLoopThreadPool(CEventLoop *baseloop,const std::string &nameArg)
:baseLoop_(baseloop)
,name_(nameArg)
,started_(false)
,numThreads_(0)
,next_(0)
{

}

CEventLoopThreadPool::~CEventLoopThreadPool()
{


}


void CEventLoopThreadPool::start(const ThreadInitCallback &cb)
{
    started_ = true;
    for (int i = 0; i < numThreads_;++i)
    {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s-%d", name_.c_str(), i);
        CEventLoopThread *t = new CEventLoopThread(cb, buf);
        threads_.push_back(std::unique_ptr<CEventLoopThread>(t));
        loops_.push_back(t->startLoop()); // 底层创建线程，绑定一个新的EventLoop，并返回该loop的地址
    }


    // 整个服务端只有一个线程，运行着baseloop
    if(numThreads_==0 && cb)
    {
        cb(baseLoop_);
    }
}

CEventLoop * CEventLoopThreadPool::getNextLoop()
{
    CEventLoop *loop = baseLoop_;//IO线程 baseloop  
    if (!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;
        if(next_>=loops_.size())
        {
            next_ = 0;
        }
    }

    return loop;
}


std::vector<CEventLoop*> CEventLoopThreadPool::getAllLoops()
{
    if(loops_.empty())
    {
          return std::vector<CEventLoop*>(1, baseLoop_);
    }
    else
    {
        return loops_;
    }
}