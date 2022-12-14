#include<iostream>
#include "Any.h"
#include "Task.h"
#include  "Thread.h"
#include "ThreadPool.h"
#include <chrono>
#include <thread>
using uLong = unsigned long long;
using namespace std;

int sum1(int a, int b)
{
    std::this_thread::sleep_for(chrono::seconds(2));
    // 比较耗时
    return a + b;
}
int sum2(int a, int b, int c)
{
    this_thread::sleep_for(chrono::seconds(2));
    return a + b + c;
}

class MyTask : public Task
{
public:
    MyTask(int begin, int end)
        : begin_(begin)
        , end_(end)
    {}
    // 问题一：怎么设计run函数的返回值，可以表示任意的类型
    // Java Python   Object 是所有其它类类型的基类
    // C++17 Any类型
    Any run()  // run方法最终就在线程池分配的线程中去做执行了!
    {
        std::cout << "tid:" << std::this_thread::get_id()<< "begin!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        uLong sum = 0;
        for (uLong i = begin_; i <= end_; i++)
            sum += i;
        sleep(3);
        return sum;
    }

private:
    int begin_;
    int end_;
};


class DivTask
{
public:
    int testFunc(int a, int b)
    {
        std::cout<<"DivTask"<<std::endl;
        this_thread::sleep_for(chrono::seconds(3));
        return a + b;
    }
private:

};





int main()
{
#if 0
    std::cout <<"TEST_ThreadPool BEGIN"<<std::endl;


    ThreadPool g_threadpool;
     //g_threadpool.setMode(PoolMode::MODE_CACHED);
     g_threadpool.start();
 {       std::shared_ptr<Result> res1 = g_threadpool.submitTask(std::make_shared<MyTask>(1, 100000000));
        std::shared_ptr<Result> res2 = g_threadpool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        std::shared_ptr<Result> res3 = g_threadpool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
        /*pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));

        pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
        pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
*/
        // 随着task被执行完，task对象没了，依赖于task对象的Result对象也没了
        uLong sum1 = res1->get().cast<uLong>();  // get返回了一个Any类型，怎么转成具体的类型呢？
        uLong sum2 = res2->get().cast<uLong>();
        uLong sum3 = res3->get().cast<uLong>();

        // Master - Slave线程模型
        // Master线程用来分解任务，然后给各个Slave线程分配任务
        // 等待各个Slave线程执行完任务，返回结果
        // Master线程合并各个任务结果，输出
        std::cout << (sum1 + sum2 + sum3) << std::endl;
}
    std::cout <<"TEST_ThreadPool END"<<std::endl;
    return  0;
#endif
    {

       // MyTask task;
        ThreadPool pool;
        pool.start();
        /*
        std::shared_ptr<Result> res1 = pool.submitTask(std::bind(&MyTask::));
        uLong sum1 = res1->get().cast<uLong>();
        std::cout << sum1<<std::endl;*/
        future<int> r1 = pool.submitTask(sum1,1,3);
        std::cout<<"get:"<<r1.get()<< std::endl;
        DivTask div;
        //std::packaged_task<int(int,int)> task=std::bind(&DivTask::run,div,_1,_2);
        auto func1 = std::bind(&DivTask::testFunc,div, std::placeholders::_1, std::placeholders::_2);

        future<int> r2 =pool.submitTask(func1,1,2);

    }


}