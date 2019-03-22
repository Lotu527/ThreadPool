#include "ThreadPool.cpp"

#include <iostream>
#include <ctime>
#include <string>
#include <unistd.h>

using namespace ThreadPoolNS;


static std::string getCurrentTime()
{
	time_t t = time(NULL);
	char ch[64] ={0};
	strftime(ch,sizeof(ch) -1,"%Y-%m-%d %H:%M:%S",localtime(&t));
	return ch;
}
//线程任务
void ShowTime()
{
    std::cout<<"现在时间是:"<<getCurrentTime()<<std::endl;
}

void test_function()
{
    //data
    const int thread_num = 10;
    const int task_num = 100;
    ThreadPool threadPool(thread_num);
    //test_fun:
    //  AddTask
    std::cout<<"TEST_FUN:AddTask\n";
    //  AddPriorityTask
    std::cout<<"TEST_FUN:AddPriorityTask\n";
    // AddDelayTask
    std::cout<<"TEST_FUN:AddDelayTask\n";
    // AddTimerTask
    std::cout<<"TEST_FUN:AddTimerTask\n";
    threadPool.AddTimerTask(1000,ShowTime);
    for(int i=0;i<task_num;i++)
    {
        threadPool.AddTask(ShowTime);
        threadPool.AddPriorityTask(ShowTime);
        threadPool.AddDelayTask(i*100,ShowTime);
    }
    
}

int main()
{
    test_function();    

    return 0;
}

