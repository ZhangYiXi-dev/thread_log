#include <iostream>
#include <vector>
#include <time.h>
#include "zyx_thread.h"
#include "measure_time.h"

zyx::Logger::ptr log_2 = (new zyx::LoggerManager(zyx::LogLevel::Level::DEBUG, true, true))->Getlogger(); //默认写日志方式
zyx::Spinlock mutex;
void fun_1(void* t)
{
   ZYX_LOG_INFO(log_2,zyx::Thread::GetName());
}
void fun_2(void* t)
{
   int *i=(int*)t;
   ZYX_LOG_INFO(log_2,std::to_string(*i));
    delete i;
}

//回调函数不设置传入参数
void test_1()
{
    zyx::Measure_time measure;
    std::vector<zyx::Thread::ptr> thrs;
    for (int i = 0; i < 100; i++)
    {
        zyx::Thread::ptr t(new zyx::Thread(&fun_1, "thread_" + std::to_string(i)));
        thrs.push_back(t);
    }
    for(size_t i = 0; i < thrs.size(); ++i) 
    {
        thrs[i]->join();
    }
}

//回调函数设置传入参数
void test_2()
{
    zyx::Measure_time measure;
    std::vector<zyx::Thread::ptr> thrs;
    for (int i = 0; i < 100; i++)
    {
        int *p=new int(i);
        zyx::Thread::ptr t(new zyx::Thread(&fun_2, "thread_" + std::to_string(i),(void*)p));
        thrs.push_back(t);
    }
    for(size_t i = 0; i < thrs.size(); ++i) 
    {
        thrs[i]->join();
    }
}
int main()
{
    //回调函数不传入参数
    test_1();
    //回调函数传入参数
    //test_2();
    std::cout<<"complete"<<std::endl;
}