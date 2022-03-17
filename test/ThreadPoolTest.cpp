#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "ThreadPool.h"

#include <signals.h>

vdk::signal<void(int)> gResult;
std::atomic<int> tResult{0};

void resultFunction(int xResult);
TEST(ThreadPoolTest, AddOneFunctionToPool)
{
    WorkerThread::ThreadPool::getInstance().AddToPool([]()
                                                      { std::cout << "I'm a thread\n"; });
}
TEST(ThreadPoolTest, AddMultipleFunctionsToPool)
{
    for (std::size_t i = 0; i < 100; i++)
    {
        WorkerThread::ThreadPool::getInstance().AddToPool([i]()
                                                          { std::cout << "I'm a thread number: " << i << '\n'; });
    }
}

TEST(ThreadPoolTest, Test)
{

    gResult.connect(&resultFunction);

    for (std::size_t i = 0; i < 10; i++)
    {
        WorkerThread::ThreadPool::getInstance().AddToPool([i]()
                                                          {
            std::this_thread::sleep_for(std::chrono::seconds(10 + i));
            std::cout << "finsihed: " << i << " after: " << 10 + i << "s \n";
            gResult.emit(i); });
    }
    std::this_thread::sleep_for(std::chrono::minutes(5));
    EXPECT_EQ(tResult, 45);
}

void resultFunction(int xResult)
{
    tResult += xResult;
}
