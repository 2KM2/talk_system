#pragma once
#include "EventLoop.h"
#include  "TcpServer.h"
#include  "InetAddress.h"


class CDBServer
{
public:
    CDBServer(EventLoop *loop, const InetAddress &listenAddr,const std::string &nameArg);
    ~CDBServer();
    void start();
private:
    TcpServer m_server;
    EventLoop *m_loop;
};