#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "task.h"

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <queue>//priority_queue


namespace ThreadPoolNS
{
    class ThreadPool
    {
    private:
        std::atomic<uint64_t> _pending_num;
        std::atomic<uint64_t> _running_num;
        uint64_t _task_count;

        bool _stop;
        int _thread_num;
        std::vector<std::thread> _ths;

        std::mutex _mtx;
        std::condition_variable _cond;
        std::deque<Task> _queue;
        std::priority_queue<Timertask> _timer_queue;

    public:
        ThreadPool(int thread_num);
        ~ThreadPool();

        bool Start();
        bool Stop(bool graceful);

        void AddTask(const Task& task);
        void AddPriorityTask(const Task& task);
        void AddDelayTask(int timeval,const Task& task);
        void AddTimerTask(int timeval,const Task& task);

        bool isEmpty(){return _running_num > 0;}
        bool CancelTask();

        static void *ThreadFunc(void* arg)
        {
            reinterpret_cast<ThreadPool*>(arg)->process();
            return 0;
        }
        void process();
    };

} // ThreadPool


#endif