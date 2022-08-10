#pragma once
#include "noncopyable.h"
#include "socket.h"
#include "channel.h"


#include <functional>

class CEventLoop;
class CInetAddress;



class CAcceptor:noncopyable
{
public:
     using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

    void setNewConnectionCallback(const NewConnectionCallback &cb) 
    {
        newConnectionCallback_ = cb;
    }


    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();
    CEventLoop *loop_; // Acceptor用的就是用户定义的那个baseLoop，也称作mainLoop
    CSocket acceptSocket_;
    CChannel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};