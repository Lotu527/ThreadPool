#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "Task.h"

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <queue>//priority_queue
#include <iostream>

namespace ThreadPoolNS
{
    class ThreadPool
    {
    private:
        std::atomic<uint64_t> _pending_num;//等待数量
        std::atomic<uint64_t> _running_num;//运行数量
        uint64_t _task_count;//任务数量

        bool _stop;//线程池是否停止运行
        int _thread_num;//工作线程数量
        std::vector<std::thread> _ths;//工作线程

        std::mutex _mtx;//信号量
        std::condition_variable _cond;//条件变量
        std::deque<Task> _queue;//任务队列
        std::priority_queue<Timertask> _timer_queue;//定时任务队列，包括延迟任务和循环任务

    public:
        ThreadPool(int thread_num)
            :_thread_num(thread_num),_pending_num(0),_running_num(0),_task_count(0),_stop(true)
            {
                Start();//启动线程池
            }
        ~ThreadPool()
        {
            Stop(true);//停止线程池
            std::cout<<"线程池已析构"<<std::endl;
        }
        /**
         *  启动线程池，默认在构造方法中调用
         */  
        bool Start();
        /**
         * 停止线程池，默认在析构函数中调用
         */ 
        bool Stop(bool graceful);
        /**
         * 线程池是否停止
         */  
        bool isStop(){return _stop;}
        /**
         * 添加普通任务
         */ 
        void AddTask(const Task& task);
        /**
         * 添加优先任务
         */ 
        void AddPriorityTask(const Task& task);
        /**
         * 添加延迟任务
         */ 
        void AddDelayTask(int timeval,const Task& task);
        /**
         * 添加定时任务
         */  
        void AddTimerTask(int timeval,const Task& task);

        /**
         * 运行任务数量是否为0
         */
        bool isEmpty(){return _running_num == 0;}

        /**
         * 线程任务包装
         */ 
        static void *ThreadFunc(void* arg)
        {
            reinterpret_cast<ThreadPool*>(arg)->process();
            return 0;
        }
        /**
         * 工作线程运行逻辑
         */ 
        void process();
    };

} // ThreadPool


#endif