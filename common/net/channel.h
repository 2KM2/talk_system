#pragma once
#include "noncopyable.h"
#include "time_stamp.h"
#include <functional>
#include <memory>
/**
 * Channel 理解为通道，封装了sockfd和其感兴趣的event，如EPOLLIN、EPOLLOUT事件
 * 还绑定了poller返回的具体事件
*/

// shared_from_this 得到当前的智能指针
class CEventLoop;

class CChannel :noncopyable
{
public:
     using EventCallback = std::function<void()>;//事件回调
     using ReadEventCallback = std::function<void(CTimeStamp)>;//只读事件回调
    CChannel(CEventLoop *loop, int fd);
    ~CChannel();

    // fd得到poller通知以后，处理事件的,回调相对应的回调函数
    void handleEvent(CTimeStamp receiveTime);

    // 设置回调函数对象
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }


    // 防止当channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);


    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }//poller 设置

    // 设置fd相应的事件状态
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }



    // 返回fd当前的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    // one loop per thread
    CEventLoop* ownerLoop() { return loop_; }
    void remove();
private:

    void update();//调用epoll_ctl
    void handleEventWithGuard(CTimeStamp receiveTime);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    CEventLoop *loop_;//事件循环
    const int fd_;//fd,Poller监听的对象
    int events_; // 注册fd感兴趣的事件
    int revents_; // poller返回的具体发生的事件
    int index_;//当前channel在poller状态

    std::weak_ptr<void> tie_;//跨线程对象问题，什么类型可以接受
    bool tied_;
    bool eventHandling_;
    bool addedToLoop_;


    // 因为channel通道里面能够获知fd最终发生的具体的事件revents，所以它负责调用具体事件的回调操作
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
    
};//end CChannel