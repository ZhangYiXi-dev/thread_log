# thread_log（学习记录二）
包装了一个线程与日志器

**编译运行方法：**
```cpp
g++ zyx_thread.cpp log.cpp main.cpp -I ./include -o thread -lpthread
./thread
```
**一些注意的部分**

```cpp
出现段错误的一些情况：
1.写日志时未加锁保护
2.线程重复释放(调用了join，然后析构时又调用了dispatch，调用这两个时注意检查是否存在)

关于日志加锁问题：
看了一些源码，貌似加的是文件锁（一种建议锁，不去检测相当于没有，底层是共享锁与互斥锁）
在参考的代码中采用了自旋锁，思考原因因该首先此日志器主要用于输出一些报错调试信息，写的内容不多，持锁时间多，反复唤醒反而更费时。
但是在本代码中测试如果100个线程同时写，互斥锁快很多（二十多毫秒），自旋锁要一百多毫秒，所以最后采用了互斥锁。
```
**各个文件功能**

```cpp
log----日志相关代码
zyx_thread----线程封装的一些代码
mutex-----封装了几种锁与信号量，后续会继续增加
measure_time.h----运行时长测试的代码
noncopyable.h----因为线程，锁相关封装是不应该允许拷贝，赋值的，所以将相关函数delete
```

**日志器部分的调用测试**

```cpp
#include <iostream>
#include "log.h"

zyx::Logger::ptr log_1=(new zyx::LoggerManager(zyx::LogLevel::Level::DEBUG, true, true))->Getlogger(); //默认写日志方式
zyx::Logger::ptr log_2=(new zyx::LoggerManager(zyx::LogLevel::Level::DEBUG, true, {"zidingyi.txt"},"%D 行号:%l %m%n"))->Getlogger();//自定义方式

int main()
{
    ZYX_LOG_DEBUG(log_1,"addr erro");
    ZYX_LOG_INFO(log_2, "ip erro");      
}
```

```cpp
// 配置一：该方式为默认方式，默认向控制台，test.txt文件按格式"%D %r-%p-%t %m%n"输出
  //参数一：日志器级别 可选： 
        /// DEBUG 级别
        //DEBUG = 1,
        /// INFO 级别
        //INFO = 2,
        /// WARN 级别
        //WARN = 3,
        /// ERROR 级别
        // ERROR = 4,
        /// FATAL 级别
        //FATAL = 5
  //参数二：是否向控制台输出，true为输出
  //参数三：是否向指定默认文件输出，true为输出
  zyx::Logger::ptr log_1=(new zyx::LoggerManager(zyx::LogLevel::Level::DEBUG, true, true))->Getlogger();
  
 // 配置二：该方式为自定义输出方式
  //参数一、二同上
  //参数三：向指定文件（可以多个）中输出
  //参数四：自定义输出格式
            //%p:输出事件level
            //%r:输出运行事件
            //%m:输出自己加入的信息（如IP error）
            //%t:输出线程号
            //%n：换行
            //%f:输出文件名
            //%l:输出行号
            //%N：输出线程名称
            //%T:输出tab
            //%F:输出协程号
            //%D:输出日期
  zyx::Logger::ptr log_2=(new zyx::LoggerManager(zyx::LogLevel::Level::DEBUG, true, {"zidingyi.txt","zidingyi2.txt"},"%D 行号:%l %m%n"))->Getlogger();//自定义方式
  
  //注意，要输出"addr erro"，"ip erro"格式里要加%m
  //可选方式有 ZYX_LOG_DEBUG(),ZYX_LOG_LEVEL_WARN(),ZYX_LOG_INFO(),ZYX_LOG_LEVEL_ERROR(),ZYX_LOG_LEVEL_FATAL()
   ZYX_LOG_DEBUG(log_1,"addr erro");
   ZYX_LOG_INFO(log_2, "ip erro");
  
  //ZYX_LOG_DEBUG ZYX_LOG_INFO为宏定义(文件中已经定义好，不需要再定义)
```
 
**线程+日志器的调用测试**

```cpp
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
```

