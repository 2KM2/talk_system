#pragma once

#include "noncopyable.h"

#include "time_stamp.h"

#include <vector>

#include <unordered_map>

class CChannel;
class CEventLoop;
// muduo库中多路事件分发器的核心IO复用模块
class CPoller:noncopyable
{
public:
    using ChannelList = std::vector<CChannel*>;

    CPoller(CEventLoop *loop);
    virtual ~CPoller() =default;

    // 给所有IO复用保留统一的接口
    virtual CTimeStamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(CChannel *channel) = 0;
    virtual void removeChannel(CChannel *channel) = 0;


    // 判断参数channel是否在当前Poller当中
    bool hasChannel(CChannel *channel) const;

    // EventLoop可以通过该接口获取默认的IO复用的具体实现
    static CPoller* newDefaultPoller(CEventLoop *loop);
protected:
    // map的key：sockfd  value：sockfd所属的channel通道类型
    using ChannelMap = std::unordered_map<int, CChannel*>;
    ChannelMap channels_;
private:
    CEventLoop *ownerLoop_; // 定义Poller所属的事件循环EventLoop
};
