#pragma once
//封装socket地址类型

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
class CInetAddress
{
public:
    explicit CInetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit CInetAddress(const sockaddr_in &addr)
    :addr_(addr)
    {

    }

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getSockAddr()const {return &addr_;}
    void  setSockAddr(const sockaddr_in &addr) { addr_ = addr; }
private:
    sockaddr_in addr_;
};