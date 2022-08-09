#pragma once
#include "poller.h"
#include "time_stamp.h"
#include "epoll_poller.h"
#include <vector>
#include <sys/epoll.h>




class CChannel;


/**
 * epoll的使用  
 * epoll_create
 * epoll_ctl   add/mod/del
 * epoll_wait
 */ 

class CEpollPoller:public CPoller
{
public:
    CEpollPoller(CEventLoop *loop);// epoll_create
    ~CEpollPoller() override;
    // 重写基类Poller的抽象方法
    CTimeStamp poll(int timeoutMs, ChannelList *activeChannels) override; //epoll_wait
    void updateChannel(CChannel *channel) override; //epoll_ctl
    void removeChannel(CChannel *channel) override; //epoll_ctl
private:
    static const int kInitEventListSize = 16;
    
    // 填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // 更新channel通道
    void update(int operation, CChannel *channel);

    int epollfd_;
    using EventList = std::vector<epoll_event>;
    EventList events_;
};