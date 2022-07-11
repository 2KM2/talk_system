#include "poller.h"

#include "channel.h"

CPoller::CPoller(CEventLoop *loop)
:ownerLoop_ (loop)
{

}


bool CPoller::hasChannel(CChannel *channel) const
{
    auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}