#pragma once
#include "TcpServer.h"
#include  "EventLoop.h"

class Chatserver
{
public:



private:
    // 上报链接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &,Timestamp);

    // 上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &,Buffer *,Timestamp);

private:
    TcpServer server_;
    EventLoop *ioLoop_;
};