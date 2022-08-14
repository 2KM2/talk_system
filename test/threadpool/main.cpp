#include<iostream>
#include  "Thread.h"
#include "ThreadPool.h"
int main()
{

    std::cout <<"TEST_ThreadPool BEGIN"<<std::endl;


    ThreadPool g_threadpool;
    g_threadpool.start();

    while(true)
    {
        sleep(1);
    }
    std::cout <<"TEST_ThreadPool END"<<std::endl;
    return  0;
}