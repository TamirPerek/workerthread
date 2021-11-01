#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../src/ThreadPool.h"

#include <signals.h>

vdk::signal<void(int)> gResult;
std::atomic<int> tResult{0};

void resultFunction(int xResult);

TEST(ThreadPoolTest, Test)
{
    
    gResult.connect(&resultFunction);
    
    for(std::size_t i = 0; i < 10; i++)
    {
        WorkerThread::ThreadPool::getInstance().AddToPool([i]() {
            std::this_thread::sleep_for(std::chrono::seconds(10 + i));
            std::cout << "finsihed: " << i << " after: " << 10 + i << "s \n";
            gResult.emit(i);
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(60));
    EXPECT_EQ(tResult, 45);
}

void resultFunction(int xResult)
{
    tResult += xResult;
}
