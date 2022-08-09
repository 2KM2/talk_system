#pragma once

#include "noncopyable.h"
#include "time_stamp.h"

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

class CChannel;
class CPoller;

//时间循环类 主要就是channel  poller抽象
/*
一堆channel
*/
class CEventLoop:noncopyable
{
public:
    using Functor = std::function<void()>;
    CEventLoop();
    ~CEventLoop();   
    // 开启事件循环
    void loop();
    // 退出事件循环
    void quit(); 


    CTimeStamp  pollReturnTime() const { return pollReturnTime_;}
private:
    using ChannelList = std::vector<CChannel*>;
    
    std::atomic_bool looping_;  // 原子操作，通过CAS实现的
    std::atomic_bool quit_; // 标识退出loop循环



    CTimeStamp pollReturnTime_; // poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;

    ChannelList activeChannels_;//激活的channel
};