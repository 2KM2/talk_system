#pragma once

#include "noncopyable.h"

#include "acceptor.h"
#include "simple_log.h"
#include "event_loop.h"
#include "eventloop_thread.h"
#include  "eventloop_threadpool.h"
#include  "inet_address.h"
#include "callbacks.h"

#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>


class CTcpServer:noncopyable    
{
public:
    using ThreadInitCallback = std::function<void(CEventLoop *)>;
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };
    CTcpServer(CEventLoop * loop,const CInetAddress &listenAddr,const std::string &nameArg,Option option = kReusePort);
    ~CTcpServer();


    void setThreadInitcallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

    // 设置底层subloop的个数
    void setThreadNum(int numThreads=10);

    // 开启服务器监听
    void start();

private:
    void newConnection(int sockfd, const CInetAddress &peerAddr);
    void removeConnection(const CTcpConnectionPtr &conn);
    void removeConnectionInLoop(const CTcpConnectionPtr &conn);

private:
    using ConnectionMap = std::unordered_map<std::string, CTcpConnectionPtr>;

    CEventLoop *loop_;// baseLoop 用户定义的loop
    const std::string ipPort_;
    const std::string  name_;

    std::unique_ptr<CAcceptor>  acceptor_; // 运行在mainLoop，任务就是监听新连接事件
    
    std::shared_ptr<CEventLoopThreadPool> threadPool_;// one loop per thread

    ConnectionCallback connectionCallback_; // 有新连接时的回调
    MessageCallback messageCallback_; // 有读写消息时的回调
    WriteCompleteCallback writeCompleteCallback_; // 消息发送完成以后的回调

    ThreadInitCallback threadInitCallback_; // loop线程初始化的回调

    std::atomic_int started_;

    int nextConnId_;
    ConnectionMap connections_; // 保存所有的连接

};