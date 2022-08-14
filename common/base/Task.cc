#include "Task.h"



Result::Result(std::shared_ptr<Task> task, bool isValid)
: isValid_(isValid)
, task_(task)
{
	task_->setResult(this);
}

void Task::exec()
{
   	if (result_ != nullptr)
	{
		result_->setVal(run()); // 这里发生多态调用
	}
}



Any Result::get() // 用户调用的
{
	if (!isValid_)
	{
		return "";
	}
	sem_.wait(); // task任务如果没有执行完，这里会阻塞用户的线程
	return std::move(any_);
}

void Result::setVal(Any any)  // Task::exec()
{
	// 存储task的返回值
	this->any_ = std::move(any);
	sem_.post(); // 已经获取的任务的返回值，增加信号量资源
}