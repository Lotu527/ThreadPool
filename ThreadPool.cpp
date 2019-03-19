#ifndef ThREAD_POOL_CPP
#define THREAD_POOL_CPP

#include "ThreadPool.h"


namespace ThreadPoolNS
{
    bool ThreadPool::Start()
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _stop = false;
        for(int i = 0;i<_thread_num;i++)
        {
            _ths.push_back(std::thread(&ThreadFunc,this));
        }
    }

    bool ThreadPool::Stop(bool graceful)
    {
        if(graceful)
        {
            while(_pending_num)
            {
                std::chrono::microseconds duration(5000);
                std::this_thread::sleep_for(duration);
            }
        }

        _stop = true;
        _cond.notify_all();
        for(int i=0;i<_thread_num;i++)
        {
            _ths[i].join();
        }
        _pending_num = _running_num = _task_count = 0;
    }

    void ThreadPool::process()
    {
        while((true)){
            std::unique_lock<std::mutex> lock(_mtx);
            
            while(_timer_queue.empty() && _queue.empty() && !stop)
            {
                _cond.wait(lock);
            }

            if(_stop)
            {
                break;
            }

            if(!_timer_queue.empty())
            {
                int64_t nowtime = time(NULL);
                Timertask newtask = _timer_queue.top();
                if(newtask._exec_time <= nowtime)
                {
                    _timer_queue.pop();
                    Task task = newtask._task;
                    bool flag = newtask._flag;
                    int64_t timeval = newtask._timeval;
                    if(flag)
                    {
                        Timertask task1(task,timeval,true);
                        _timer_queue.push(newtask);
                        _task_count++;
                    }

                    _running_num++;
                    _pending_num--;
                    lock.unlock();
                    task();
                    lock.lock();
                    _running_num--;
                }
            }
            
            if(!_queue.empty())
            {
                Task task = _queue.front();
                _queue.pop_front();
                _pending_num--;
                _running_num++;
                lock.unlock();
                task();
                lock.lock();
                _running_num--;
            }
        }
    }

    void ThreadPool::AddTask(const Task& task)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _queue.push_back(task);
        _pending_num++;
        _task_count++;
        _cond.notify_one();
    }

    void ThreadPool::AddPriorityTask(const Task& task)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _queue.push_front(task);
        _pending_num++;
        _task_count++;
        _cond.notify_one();
    }

    void ThreadPool::AddDelayTask(int timeval,const Task& task)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        Timertask newtask(task,timeval);
        _timer_queue.push(newtask);
        _task_count++;
        _cond.notify_one();
    }

    void ThreadPool::AddTimerTask(int timeval,const Task& task)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        Timertask newtask(task,timeval,true);
        _timer_queue.push(newtask);
        _task_count++;
        _cond.notify_one();
    }

}




#endif