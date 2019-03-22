#ifndef THREAD_POOL_CPP
#define THREAD_POOL_CPP

#include "ThreadPool.h"


namespace ThreadPoolNS
{
    bool ThreadPool::Start()
    {
        
        std::unique_lock<std::mutex> lock(_mtx);
        _stop = false;
        //初始化线程任务
        for(int i = 0;i<_thread_num;i++)
        {
            _ths.push_back(std::thread(&ThreadFunc,this));
        }
        return true;
    }

    /**
     * graceful，线程池停止模式
     *  true,宽容模式，等待所有任务执行再退出
     *  false,严格模式，立刻终止所有任务运行并退出
     */ 
    bool ThreadPool::Stop(bool graceful)
    {
        
        if(graceful)
        { 
            //等待排队任务执行完毕
            while(_pending_num)
            {
                std::chrono::microseconds duration(1000);
                std::this_thread::sleep_for(duration);
                //std::cout<<"  pending = "<<_pending_num<<std::endl;
            }
        }

        /**
         * 标志结束运行
         * 唤醒所有等待线程
         * 线性的运行所有任务
         */ 
        _stop = true;
        _cond.notify_all();
        for(int i=0;i<_thread_num;i++)
        {
            _ths[i].join();
        }
        std::cout<<"任务数量:"<<_task_count<<std::endl;
        std::cout<<"等待执行:"<<_pending_num<<std::endl;
        std::cout<<"正在执行:"<<_running_num<<std::endl;
        _pending_num = _running_num = _task_count = 0;
        return true;
    }

    void ThreadPool::process()
    {
        while(true){
            std::unique_lock<std::mutex> lock(_mtx);
            //等待定时任务或普通任务
            while(_timer_queue.empty() && _queue.empty() && !_stop)
            {
                _cond.wait(lock);
            }
            //线程池停止后，工作线程停止运行
            if(_stop)
            {
                break;
            }
            //首先执行定时任务
            if(!_timer_queue.empty())
            {
                //获取当前时间
                int64_t nowtime = time(NULL)*1000;
                //获取定时任务
                Timertask newtask = _timer_queue.top();
                //如果定时任务的执行时间已到或已过
                if(newtask._exec_time <= nowtime)
                {
                    _timer_queue.pop();
                    Task task = newtask._task;
                    bool flag = newtask._flag;
                    int64_t timeval = newtask._timeval;
                    //如果是循环定时任务
                    if(flag)
                    {
                        //定时任务队列中再次添加此任务
                        Timertask taskcircle(task,timeval,true);
                        _timer_queue.push(taskcircle);
                        _task_count++;
                        _pending_num++;
                    }
                    
                    _running_num++;
                    _pending_num--;
                    //释放锁
                    lock.unlock();
                    //执行任务
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
        _pending_num++;
        _task_count++;
        _cond.notify_one();
    }

    void ThreadPool::AddTimerTask(int timeval,const Task& task)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        Timertask newtask(task,timeval,true);
        _timer_queue.push(newtask);
        _pending_num++;
        _task_count++;
        _cond.notify_one();
    }

}




#endif