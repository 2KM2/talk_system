#pragma once
#include "TcpServer.h"
#include  "EventLoop.h"
#include  "json/json.h"
#include <iostream>
using json = nlohmann::json;
class MsgServer
{
public:
    MsgServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const std::string &nameArg);
            
private:
    // 上报链接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &);

    // 上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &,Buffer *,Timestamp);

private:
    TcpServer server_;
    EventLoop *ioLoop_;
};