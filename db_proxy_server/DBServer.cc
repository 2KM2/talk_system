#include "DBServer.h"


CDBServer::CDBServer(EventLoop *loop, const InetAddress &listenAddr,const std::string &nameArg)
:m_server(loop,listenAddr,nameArg)
,m_loop(loop)
{

    m_server.setThreadNum(48);
}





   
void CDBServer::start()
{
    m_server.start();
}

CDBServer::~CDBServer()
{

}