#ifndef THREAD_POOL_TASK_H
#define THREAD_POOL_TASK_H

#include<functional>
#include<ctime>


namespace ThreadPoolNS
{
    using Task = std::function<void ()>;
    
    struct Timertask{

    bool _flag;
    Task _task;
    int64_t _timeval;
    int64_t _exec_time;

    bool operator<(const struct Timertask& other) const{
        return _exec_time > other._exec_time;
    }

    Timertask(const Task& task,int64_t timeval,int64_t exec_time,bool flag = false)
        :_flag(flag),_task(task),_timeval(timeval),_exec_time(exec_time)
        {}

    Timertask(const Task& task,int64_t timeval,bool flag = false)
        :Timertask(task,timeval,timeval+time(NULL),flag)
        {}
    
    };
} // ThreadPool




#endif
