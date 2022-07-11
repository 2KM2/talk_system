#include "acceptor.h"
#include  "simple_log.h"
#include  "inet_address.h"


#include <sys/types.h>    
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
static int createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0) 
    {
        LOG_FATAL("%s:%s:%d listen socket create err:%d \n", __FILE__, __FUNCTION__, __LINE__, errno);
    }

    return sockfd;
}



 CAcceptor::CAcceptor(CEventLoop *loop, const CInetAddress &listenAddr, bool reuseport)
 :loop_(loop)
 ,acceptSocket_(createNonblocking())//socket
 ,acceptChannel_(loop,acceptSocket_.fd())
 ,listenning_(false)
 {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr); // bind
    // TcpServer::start() -> Acceptor.listen  有新用户的连接，要执行一个回调（connfd-> channel-> subloop）
    // baseLoop => acceptChannel_(listenfd) => 
    acceptChannel_.setReadCallback(std::bind(&CAcceptor::handleRead, this));
 }


CAcceptor::~CAcceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void CAcceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen(); // listen
    acceptChannel_.enableReading(); // acceptChannel_ => Poller
}

// listenfd有事件发生了，就是有新用户连接了
void CAcceptor::handleRead()
{
    CInetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (newConnectionCallback_)//TcpServer的函数
        {
            newConnectionCallback_(connfd, peerAddr); // 轮询找到subLoop，唤醒，分发当前的新客户端的Channel
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LOG_ERROR("%s:%s:%d accept err:%d \n", __FILE__, __FUNCTION__, __LINE__, errno);
        if (errno == EMFILE)
        {
            LOG_ERROR("%s:%s:%d sockfd reached limit! \n", __FILE__, __FUNCTION__, __LINE__);
        }
    }

}