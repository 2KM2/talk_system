#pragma once
#include "Thread.h"
#include  "Any.h"
#include  "Task.h"
#include  "noncopyable.h"
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <thread>
#include <future>

enum PoolMode
{
    MODE_FIXED,//固定数量的线程
    MODE_CACHED, //线程数量可动态增长
};







class ThreadPool: noncopyable
{

public:
    ThreadPool();
    ~ThreadPool();

    void start(int initThreadSize = std::thread::hardware_concurrency());
	// 设置线程池的工作模式
	void setMode(PoolMode mode);
    // 设置task任务队列上线阈值
	void setTaskQueMaxThreshHold(int threshhold);
	// 设置线程池cached模式下线程阈值
	void setThreadSizeThreshHold(int threshhold);
	// 检查pool的运行状态
	bool checkRunningState() const;

    // 给线程池提交任务
    Result submitTask(std::shared_ptr<Task> sp);
private:
    void threadFunc(int threadid);
private:
    std::unordered_map<int,std::shared_ptr<Thread>> threads_;//线程列表
    int  initThreadSize_;//初始的线程数量
    int threadSizeThreshHold_; // 线程数量上限阈值
    std::atomic_int curThreadSize_;	// 记录当前线程池里面线程的总数量
	std::atomic_int idleThreadSize_; // 记录空闲线程的数量


    std::queue<std::shared_ptr<Task>> taskQue_;//任务队列
    std::atomic_int taskSize_; // 任务的数量
	int taskQueMaxThreshHold_;  // 任务队列数量上限阈值


    std::mutex taskQueMtx_; // 保证任务队列的线程安全
	std::condition_variable notFull_; // 表示任务队列不满
	std::condition_variable notEmpty_; // 表示任务队列不空
	std::condition_variable exitCond_; // 等到线程资源全部回收

	PoolMode poolMode_; // 当前线程池的工作模式
	std::atomic_bool isPoolRunning_; // 表示当前线程池的启动状态
};


