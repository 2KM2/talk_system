#include "eventloop_thread.h"
#include "event_loop.h"


CEventLoopThread::CEventLoopThread(const ThreadInitCallback &cb,const std::string &name)
:loop_(nullptr)
,exiting_(false)
,thread_(std::bind(&CEventLoopThread::threadFunc,this),name)
,mutex_()
,cond_()
,callback_(cb)
{

}


CEventLoopThread::~CEventLoopThread()
{
    exiting_ = true;
    if (loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}
//返回新线程中的真正的EventLoop
CEventLoop* CEventLoopThread::startLoop()
{
    thread_.start(); // 启动底层的新线程
    CEventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while ( loop_ == nullptr )
        {
            cond_.wait(lock);
        }
        loop = loop_;
    }

    return loop;
}

// 下面这个方法，实在单独的新线程里面运行的
void CEventLoopThread::threadFunc()
{
    //线程栈上 不用析构
    CEventLoop  loop; // 创建一个独立的eventloop，和上面的线程是一一对应的，one loop per thread
    if (callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop(); // EventLoop loop  => Poller.poll,进入阻塞状态
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;//服务器程序已经关闭了
}