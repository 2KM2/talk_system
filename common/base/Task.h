#pragma once
#include "Any.h"
#include "Semaphore.h"
#include <atomic>
#include <iostream>
class Task;
// 实现接收提交到线程池的task任务执行完成后的返回值类型Result
class Result
{
public:
	explicit Result(std::shared_ptr<Task> task, bool isValid = true);
	~Result() = default;
	// 问题一：setVal方法，获取任务执行完的返回值的
	void setVal(Any any);
	// 问题二：get方法，用户调用这个方法获取task的返回值
	Any get();
private:
	Any any_; // 存储任务的返回值
	Semaphore sem_; // 线程通信信号量
	std::shared_ptr<Task> task_; //指向对应获取返回值的任务对象 
	std::atomic_bool isValid_; // 返回值是否有效

};
class Task
{
public:
	Task();
	~Task() = default;
	void exec();
	void setResult(Result* res);

	// 用户可以自定义任意任务类型，从Task继承，重写run方法，实现自定义任务处理
	virtual Any run() = 0;

private:
	Result* result_; // Result对象的声明周期 大于 Task的
};


