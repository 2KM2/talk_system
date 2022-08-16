
#include "msgserver.h"
#include <iostream>


MsgServer::MsgServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const std::string &nameArg)
{

    server_->setThreadNum(4);

}







void MsgServer::onConnection(const TcpConnectionPtr & conn)
{

    if(!conn->connected())
    {
        conn->shutdown();
    }

}


void MsgServer::onMessage(const TcpConnectionPtr &conn,Buffer * buffer,Timestamp time)
{
    std::string buf =buffer->retrieveAllAsString();
    // 数据的反序列化 
    json js = json::parse(buf);


}