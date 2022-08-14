#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <memory>
// 实现一个信号量类
class Semaphore
{
public:
	Semaphore(int limit = 0) 
		:resLimit_(limit)
	{}
	~Semaphore() = default;

	// 获取一个信号量资源
	void wait()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		// 等待信号量有资源，没有资源的话，会阻塞当前线程
		cond_.wait(lock, [&]()->bool {return resLimit_ > 0; });
		resLimit_--;
	}

	// 增加一个信号量资源
	void post()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		resLimit_++;
		// linux下condition_variable的析构函数什么也没做
		// 导致这里状态已经失效，无故阻塞
		cond_.notify_all();  // 等待状态，释放mutex锁 通知条件变量wait的地方，可以起来干活了
	}
private:
	int resLimit_;
	std::mutex mtx_;
	std::condition_variable cond_;
};