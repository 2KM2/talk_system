#pragma once
#include <memory>
#include "TcpConnection.h"
using namespace std;


class TcpSession
{
public:
    TcpSession(const std::weak_ptr<TcpConnection>& tmpconn);
    ~TcpSession();

    TcpSession(const TcpSession& rhs) = delete;
    TcpSession& operator =(const TcpSession& rhs) = delete;

    std::shared_ptr<TcpConnection> getConnectionPtr()
    {
        if (tmpConn_.expired())
            return NULL;

        return tmpConn_.lock();
    }

    void send(int32_t cmd, int32_t seq, const std::string& data);
    void send(int32_t cmd, int32_t seq, const char* data, int32_t dataLength);
    void send(const std::string& p);
    void send(const char* p, int32_t length);

private:
    void sendPackage(const char* p, int32_t length);
protected:
    std::weak_ptr<TcpConnection> tmpConn_;
};
