#include "ThreadPool.h"


const int TASK_MAX_THRESHHOLD = INT32_MAX; // INT32_MAX;
const int THREAD_MAX_THRESHHOLD = 1024;
const int THREAD_MAX_IDLE_TIME = 60; // 单位：秒



ThreadPool::ThreadPool()
    :initThreadSize_(0)
    ,taskSize_(0)
    ,idleThreadSize_(0)
    ,curThreadSize_(0)
    ,taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD)
    ,threadSizeThreshHold_(THREAD_MAX_THRESHHOLD)
    ,poolMode_(PoolMode::MODE_FIXED)
    ,isPoolRunning_(false)
    {

    }


ThreadPool::~ThreadPool()
{
    isPoolRunning_ = false;

    // 等待线程池里面所有的线程返回  有两种状态：阻塞 & 正在执行任务中
	std::unique_lock<std::mutex> lock(taskQueMtx_);
	notEmpty_.notify_all();
	exitCond_.wait(lock, [&]()->bool {return threads_.size() == 0; });
}


void ThreadPool::start(int initThreadSize)
{
    //设置线程运行状态
    isPoolRunning_=true;

    //记录初始线程个数
    initThreadSize_ = initThreadSize;
    curThreadSize_ = initThreadSize;

    for(int i=0;i<initThreadSize; ++i)
    {
        //创建thread线程对象时候,把线程函数给到thread线程对象
        auto ptr=std::make_shared<Thread>(std::bind(&ThreadPool::threadFunc,this,std::placeholders::_1));
        int threadId = ptr->numCreated();
        threads_.emplace(threadId,std::move(ptr));
    }

    //启动所有线程

    for(int i=0;i<initThreadSize; ++i)
    {
        threads_[i]->start();//需要去执行一个线程函数
        idleThreadSize_++;// 记录初始空闲线程的数量
    }
}

void ThreadPool::setMode(PoolMode mode)
{
	if (checkRunningState())
		return;
	poolMode_ = mode;
}

void ThreadPool::setTaskQueMaxThreshHold(int threshhold)
{
	if (checkRunningState())
		return;
	taskQueMaxThreshHold_ = threshhold;
}

void ThreadPool::setThreadSizeThreshHold(int threshhold)
{
	if (checkRunningState())
		return;
	if (poolMode_ == PoolMode::MODE_CACHED)
	{
		threadSizeThreshHold_ = threshhold;
	}
}

bool ThreadPool::checkRunningState() const
{
	return isPoolRunning_;
}

// 定义线程函数   线程池的所有线程从任务队列里面消费任务
void ThreadPool::threadFunc(int threadid) // 线程函数返回，相应的线程也就结束了
{
   //std::cout <<"ThreadPool::threadFunc start "<<threadid<< std::endl;
   auto lastTime = std::chrono::high_resolution_clock().now();
	// 所有任务必须执行完成，线程池才可以回收所有线程资源
    for(;;)
    {
        std::shared_ptr<Task> task;
       {
			// 先获取锁
			std::unique_lock<std::mutex> lock(taskQueMtx_);

			std::cout << "tid:" << std::this_thread::get_id()
				<< "尝试获取任务..." << std::endl;

			// cached模式下，有可能已经创建了很多的线程，但是空闲时间超过60s，应该把多余的线程
			// 结束回收掉（超过initThreadSize_数量的线程要进行回收）
			// 当前时间 - 上一次线程执行的时间 > 60s
			
			// 每一秒中返回一次   怎么区分：超时返回？还是有任务待执行返回
			// 锁 + 双重判断
			while (taskQue_.size() == 0)
			{
				// 线程池要结束，回收线程资源
				if (!isPoolRunning_)
				{
					threads_.erase(threadid); // std::this_thread::getid()
					std::cout << "threadid:" << std::this_thread::get_id()<<" threadid:"<<threadid << " exit! "<< std::endl;
					exitCond_.notify_all();
					return; // 线程函数结束，线程结束
				}

				if (poolMode_ == PoolMode::MODE_CACHED)
				{
					// 条件变量，超时返回了
					if (std::cv_status::timeout ==
						notEmpty_.wait_for(lock, std::chrono::seconds(1)))
					{
						auto now = std::chrono::high_resolution_clock().now();
						auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
						if (dur.count() >= THREAD_MAX_IDLE_TIME
							&& curThreadSize_ > initThreadSize_)
						{
							// 开始回收当前线程
							// 记录线程数量的相关变量的值修改
							// 把线程对象从线程列表容器中删除   没有办法 threadFunc《=》thread对象
							// threadid => thread对象 => 删除
							threads_.erase(threadid); // std::this_thread::getid()
							curThreadSize_--;
							idleThreadSize_--;

							std::cout << "threadid:" << std::this_thread::get_id() << " exit!" << std::endl;
							return;
						}
					}
				}
				else
				{
					// 等待notEmpty条件
					notEmpty_.wait(lock);
				}
			}

			idleThreadSize_--;

			std::cout << "tid:" << std::this_thread::get_id()<< "获取任务成功..." << std::endl;

		task = taskQue_.front();
		taskQue_.pop();
		taskSize_--;

        // 如果依然有剩余任务，继续通知其它得线程执行任务
		if (taskQue_.size() > 0)
		{
			notEmpty_.notify_all();
		}
        // 取出一个任务，进行通知，通知可以继续提交生产任务
		notFull_.notify_all();
       }// 就应该把锁释放掉

        
        // 当前线程负责执行这个任务     
        if (task != nullptr)    
        {      
            	// task->run(); // 执行任务；把任务的返回值setVal方法给到Result     
        	task->exec();       
        }

	    idleThreadSize_++;
		lastTime = std::chrono::high_resolution_clock().now(); // 更新线程执行完任务的时间

    }
    std::cout <<"ThreadPool:threadFunc exit-"<< threadid <<std::endl;
}



std::shared_ptr<Result> ThreadPool::submitTask(std::shared_ptr<Task> sp)
{
	// 获取锁
	std::unique_lock<std::mutex> lock(taskQueMtx_);

	// 线程的通信  等待任务队列有空余   wait   wait_for   wait_until
    /**
     * wait 一直等待
     * wait_for 时间
     * wait_until 时间截点
    */
	// 用户提交任务，最长不能阻塞超过1s，否则判断提交任务失败，返回
    //服务降级
    if (!notFull_.wait_for(lock, std::chrono::seconds(1),
		[&]()->bool { return taskQue_.size() < (size_t)taskQueMaxThreshHold_; }))
	{
		// 表示notFull_等待1s种，条件依然没有满足
		std::cerr << "task queue is full, submit task fail." << std::endl;
		return std::make_shared<Result>(sp,false);
	}

	// 如果有空余，把任务放入任务队列中
	taskQue_.emplace(sp);
	taskSize_++;


    // 因为新放了任务，任务队列肯定不空了，在notEmpty_上进行通知，赶快分配线程执行任务
	notEmpty_.notify_all();


    // cached模式 任务处理比较紧急
    // 场景：小而快的任务 需要根据任务数量和空闲线程的数量，判断是否需要创建新的线程出来
	if (poolMode_ == PoolMode::MODE_CACHED
		&& taskSize_ > idleThreadSize_
		&& curThreadSize_ < threadSizeThreshHold_)
	{
   
        // 创建新的线程对象
		auto ptr = std::make_shared<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
		int threadId = ptr->numCreated();
        std::cout << ">>> create new thread..."<<threadId << std::endl;
		threads_.emplace(threadId, std::move(ptr));
		// 启动线程
		threads_[threadId]->start(); 
		// 修改线程个数相关的变量
		curThreadSize_++;
		idleThreadSize_++;
    }

    return std::make_shared<Result>(sp); 
}