#pragma once
#include "poller.h"
#include "time_stamp.h"
#include <vector>
#include <sys/epoll.h>




class CChannel;


/**
 * epoll的使用  
 * epoll_create
 * epoll_ctl   add/mod/del
 * epoll_wait
 */ 

class CEpollPoller:public CPoller
{
public:
    

};