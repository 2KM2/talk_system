//好的设计
#include "poller.h"
#include "epoll_poller.h"
#include <stdlib.h>




CPoller *CPoller::newDefaultPoller(CEventLoop *loop)
{

#ifdef USE_POLL
    return nullptr;//未实现
#endif

    return new CEpollPoller(loop);//生产epoll的实列
}