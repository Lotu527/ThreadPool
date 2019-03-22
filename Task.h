#ifndef THREAD_POOL_TASK_H
#define THREAD_POOL_TASK_H

#include<functional>
#include<ctime>


namespace ThreadPoolNS
{
    /**
     * 任务仿函数
     */ 
    using Task = std::function<void ()>;
    
    /**
     * 定时任务
     */ 
    struct Timertask{

    bool _flag;//是否是重复任务
    Task _task;//任务函数指针
    int64_t _timeval;//延迟时间,100ms
    int64_t _exec_time;//执行时间,100ms

    bool operator<(const struct Timertask& other) const{
        return _exec_time > other._exec_time;
    }

    Timertask(const Task& task,int64_t timeval,int64_t exec_time,bool flag = false)
        :_flag(flag),_task(task),_timeval(timeval),_exec_time(exec_time)
        {}

    Timertask(const Task& task,int64_t timeval,bool flag = false)
        :Timertask(task,timeval,timeval+time(NULL)*1000,flag)
        {}
    
    };
} // ThreadPool




#endif
