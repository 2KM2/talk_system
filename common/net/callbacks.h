#pragma once
#include <memory>
#include <functional>


class CBuffer;
class CTcpConnection;
class CTimeStamp;

using CTcpConnectionPtr=std::shared_ptr<CTcpConnection>;

using ConnectionCallback = std::function<void (const CTcpConnectionPtr&)>;
using CloseCallback = std::function<void (const CTcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void (const CTcpConnectionPtr&)>;


using MessageCallback = std::function<void (const CTcpConnectionPtr&, CBuffer*,CTimeStamp)>;
using HighWaterMarkCallback = std::function<void (const CTcpConnectionPtr&, size_t)>;


