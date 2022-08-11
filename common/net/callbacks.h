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

/**
 * 处理策略：到达水位线可以暂停发送
*/
using HighWaterMarkCallback = std::function<void (const CTcpConnectionPtr&, size_t)>;


