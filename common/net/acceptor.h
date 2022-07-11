#pragma once
#include "noncopyable.h"
#include "socket.h"
#include "channel.h"


#include <functional>

class CEventLoop;
class CInetAddress;


//运行再mainloop
class CAcceptor:noncopyable
{
public:
     using NewConnectionCallback = std::function<void(int sockfd, const CInetAddress&)>;
    CAcceptor(CEventLoop *loop, const CInetAddress &listenAddr, bool reuseport);
    ~CAcceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb) 
    {
        newConnectionCallback_ = std::move(cb);
    }
    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();
    CEventLoop *loop_; // Acceptor用的就是用户定义的那个baseLoop，也称作mainLoop
    CSocket acceptSocket_;
    CChannel acceptChannel_;//打包的fd就是 acceptSocket_
    NewConnectionCallback newConnectionCallback_;//处理新连接,打包成channel 分发给subloop
    bool listenning_;
};