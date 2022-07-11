#include "tcp_server.h"
#include "simple_log.h"
#include  "tcp_connection.h"
#include  <strings.h>
#include  <functional>

static CEventLoop* CheckLoopNotNull(CEventLoop *loop)
{
    if (loop == nullptr)
    {
        LOG_FATAL("%s:%s:%d mainLoop is null! \n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

/**
 * acceptor -->  创建socket listenfd -> 封装acceptchannel-> enableReading() --> 添加到Poller
 *
 * 
 * Poller:监听accpetchannel --> handleRead --> accept 返回 connectFd--> newConnectionCallback 
 * 
*/
CTcpServer::CTcpServer(CEventLoop * loop,
                        const CInetAddress &listenAddr,
                        const std::string &nameArg,
                        Option option)
:loop_(CheckLoopNotNull(loop))
,ipPort_(listenAddr.toIpPort())
,name_(nameArg)
,acceptor_(new CAcceptor(loop,listenAddr,option == kReusePort))
,threadPool_(new CEventLoopThreadPool(loop,name_))
,connectionCallback_()
,messageCallback_()
,nextConnId_(1)
,started_(0)
{
    // 当有先用户连接时，会执行TcpServer::newConnection回调
    acceptor_->setNewConnectionCallback(std::bind(&CTcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

CTcpServer::~CTcpServer()
{
    for (auto &item : connections_)
    {
        // 这个局部的shared_ptr智能指针对象，出右括号，可以自动释放new出来的TcpConnection对象资源了
        CTcpConnectionPtr conn(item.second); 
        item.second.reset();

        // 销毁连接
        conn->getLoop()->runInLoop(
            std::bind(&CTcpConnection::connectDestroyed, conn)
        );
    }
}
// 设置底层subloop的个数
void CTcpServer::setThreadNum(int numThreads)
{
    threadPool_->setThreadNum(numThreads);
}

// 开启服务器监听   loop.loop()
void CTcpServer::start()
{
    if (started_++ == 0) // 防止一个TcpServer对象被start多次
    {
        threadPool_->start(threadInitCallback_); // 启动底层的loop线程池

        //直接执行回调了
        loop_->runInLoop(std::bind(&CAcceptor::listen, acceptor_.get()));
    }
}

// 有一个新的客户端的连接，acceptor会执行这个回调操作
void CTcpServer::newConnection(int sockfd, const CInetAddress &peerAddr)
{
    // 轮询算法，选择一个subLoop，来管理channel
    /**
    * 每个loop都有wakeupfd mainLoop 唤醒 subLoop
    * runInLoop 
    * queueInLoop 
    */
    CEventLoop *ioLoop = threadPool_->getNextLoop(); 
    char buf[64]={0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s \n",
        name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());
    // 通过sockfd获取其绑定的本机的ip地址和端口信息
    sockaddr_in local;
    ::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;
    if (::getsockname(sockfd, (sockaddr*)&local, &addrlen) < 0)
    {
        LOG_ERROR("sockets::getLocalAddr");
    }
    InetAddress localAddr(local);
    //根据连接成功的sockfd，创建TcpConnection连接对象
    //CTcpConnectionPtr conn()



   // 直接调用TcpConnection::connectEstablished
    /**
     * 唤醒subLoop
     * runInLoop->queueInLoop->wakeupfd
    */
    ioLoop->runInLoop(std::bind(&CTcpConnection::connectEstablished, conn));
}


void CTcpServer::removeConnection(const CTcpConnectionPtr &conn)
{
        loop_->runInLoop(
        std::bind(&CTcpServer::removeConnectionInLoop, this, conn)
    );
}


void CTcpServer::removeConnectionInLoop(const CTcpConnectionPtr &conn)
{
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s\n", 
        name_.c_str(), conn->name().c_str());

    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop(); 
    ioLoop->queueInLoop(
        std::bind(&CTcpConnection::connectDestroyed, conn)
    );
}