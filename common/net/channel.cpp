#include "channel.h"
#include "event_loop.h"
#include "simple_log.h"
#include <sys/epoll.h>
#include <assert.h>
const int CChannel::kNoneEvent = 0;
const int CChannel::kReadEvent = EPOLLIN | EPOLLPRI;
const int CChannel::kWriteEvent  = EPOLLOUT;

// EventLoop: ChannelList Poller
CChannel::CChannel(CEventLoop *loop,int fd)
:loop_(loop)
,fd_(fd)
,events_(0)
,revents_(0)
,index_(-1)
,tied_(false)
,eventHandling_(false)
,addedToLoop_(false)
{

}


CChannel::~CChannel()
{
  assert(!eventHandling_);//析构时候保证没有处理事件
  assert(!addedToLoop_);//析构时候 保证被remove或者没使用
 /*
   if (loop_->isInLoopThread()) //判断当前channel是否属于当前的EventLoop
   {
     assert(!loop_->hasChannel(this));
   }
 */

}
// channel的tie方法什么时候调用过？一个TcpConnection新连接创建的时候 TcpConnection => Channel 
void CChannel::tie(const std::shared_ptr<void> &obj )
{
    tie_ = obj;
    tied_ = true;
}

/**
 * 当改变channel所表示fd的events事件后，update负责在poller里面更改fd相应的事件epoll_ctl
 * EventLoop => ChannelList   Poller
 */ 

void CChannel::update()
{
    // 通过channel所属的EventLoop，调用poller的相应方法，注册fd的events事件
    addedToLoop_ = true;
    //TODO
    loop_->updateChannel(this);
}


// 在channel所属的EventLoop中， 把当前的channel删除掉
void CChannel::remove()
{
  assert(isNoneEvent());//当前没有事件才会remove
  addedToLoop_ = false;
  // TODO
  loop_->removeChannel(this);
}

// fd得到poller通知以后，处理事件的
void CChannel::handleEvent(CTimeStamp receiveTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();//弱指针提升强指针
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }

}

//根据poller通知的channel 发生的具体事件,由channel 负责回调具体操作
void CChannel::handleEventWithGuard(CTimeStamp receiveTime)
{
    eventHandling_=true;
    LOG_INFO("channel handleEvent revents:%d\n", revents_);
    
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }


    if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
    eventHandling_=false;

}