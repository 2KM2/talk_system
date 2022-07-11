#pragma once

#include "noncopyable.h"
class CInetAddress;


//封装 socket fd

class CSocket:noncopyable
{
public:
    explicit CSocket(int sockfd)
    :sockfd_(sockfd)
    {

    }
    ~CSocket();

    int fd() const { return sockfd_; }
    void bindAddress(const CInetAddress &localaddr);
    void listen();
    int accept(CInetAddress *peeraddr);

    void shutdownWrite();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
private:
    const int sockfd_;
};
