#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <random>

#include "ThreadPool.hpp"

TEST(ThreadPoolTest, GreateInstance)
{
    ThreadPool::createInstance();
    ASSERT_TRUE(ThreadPool::isCreated());

    ThreadPool::createInstance();
    ASSERT_TRUE(ThreadPool::isCreated());
    ASSERT_EQ(ThreadPool::getInstance().getProcessorCount(), std::thread::hardware_concurrency());

    ThreadPool::createInstance(64);
    ASSERT_TRUE(ThreadPool::isCreated());
    ASSERT_EQ(ThreadPool::getInstance().getProcessorCount(), 64);
}

static constexpr int TestFunc()
{
    return 40 + 2;
}

static double calculatePi(size_t iterations)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    size_t insideCircle = 0;

    for (size_t i = 0; i < iterations; ++i)
    {
        double x = dist(gen);
        double y = dist(gen);

        if (x * x + y * y <= 1.0)
        {
            ++insideCircle;
        }
    }

    return (4.0 * insideCircle) / iterations;
}

TEST(ThreadPoolTest, AddOneFunctionToPool)
{
    ASSERT_EQ(ThreadPool::getInstance().add([]()
                                            { return 2 + 4; })
                  .get(),
              6);
}
TEST(ThreadPoolTest, AddOneStaticFunctionToPool)
{
    auto tFuture = ThreadPool::getInstance().add(&TestFunc);
    ASSERT_EQ(tFuture.get(), 42);
}

TEST(ThreadPoolTest, AddMultipleFunctionsToPool)
{
    std::vector<std::future<std::size_t>> tFutures;
    for (std::size_t i = 0; i < 100; i++)
    {
        tFutures.emplace_back(ThreadPool::getInstance().add([i]()
                                                            { 
                                        // std::cout << "I'm a thread number: " << i << '\n'; 
                                        return i; }));
    }

    std::size_t result = 0;
    for (auto &&future : tFutures)
    {
        result += future.get();
    }

    ASSERT_EQ(result, 4950);
}

TEST(ThreadPoolTest, AddMultipleStaticFunctionsToPool)
{
    std::vector<std::future<int>> tFutures;
    for (std::size_t i = 0; i < 100; i++)
    {
        tFutures.emplace_back(ThreadPool::getInstance().add(&TestFunc));
    }

    int result = 0;
    for (auto &&future : tFutures)
    {
        result += future.get();
    }

    ASSERT_EQ(result, 4200);
}

TEST(ThreadPoolTest, AddMonteCarlosToPool)
{
    std::vector<std::future<double>> tFutures;
    static constexpr std::size_t amount{100};
    for (std::size_t i = 0; i < amount; i++)
    {
        tFutures.emplace_back(ThreadPool::getInstance().add([]()
                                                            { return calculatePi(1000000); }));
    }

    double result = 0;
    for (auto &&future : tFutures)
    {
        result += future.get();
    }
    result /= amount;
    ASSERT_NEAR(result, 3.14159, 0.01) << "Pi is not correct";
}

TEST(ThreadPoolTest, ParallelProcessing)
{
    std::vector<std::future<void>> tFutures;
    std::atomic<int> counter{0};
    static constexpr std::size_t numTasks{1000};

    for (std::size_t i = 0; i < numTasks; ++i)
    {
        tFutures.emplace_back(ThreadPool::getInstance().add([&counter]()
                                                            { 
                                        for (int j = 0; j < 1000; ++j)
                                        {
                                            ++counter;
                                        } }));
    }

    for (auto &&future : tFutures)
    {
        future.get();
    }

    ASSERT_EQ(counter, numTasks * 1000);
}

TEST(ThreadPoolTest, AddLambdaFunctionToPool)
{
    auto tFuture = ThreadPool::getInstance().add([]()
                                                 { return 7 * 6; });
    ASSERT_EQ(tFuture.get(), 42);
}

TEST(ThreadPoolTest, AddMemberFunctionToPool)
{
    struct TestClass
    {
        int memberFunction()
        {
            return 123;
        }
    };

    TestClass obj;
    auto tFuture = ThreadPool::getInstance().add(&TestClass::memberFunction, &obj);
    ASSERT_EQ(tFuture.get(), 123);
}

TEST(ThreadPoolTest, AddMultipleDifferentTasks)
{
    auto future1 = ThreadPool::getInstance().add([]()
                                                 { return 1 + 1; });
    auto future2 = ThreadPool::getInstance().add([]()
                                                 { return 2 + 2; });
    auto future3 = ThreadPool::getInstance().add([]()
                                                 { return 3 + 3; });

    ASSERT_EQ(future1.get(), 2);
    ASSERT_EQ(future2.get(), 4);
    ASSERT_EQ(future3.get(), 6);
}

TEST(ThreadPoolTest, AddFunctionThrowingException)
{
    auto tFuture = ThreadPool::getInstance().add([]()
                                                 { 
                                    throw std::runtime_error("Test exception");
                                    return 0; });

    ASSERT_THROW(tFuture.get(), std::runtime_error);
}

TEST(ThreadPoolTest, AddMultipleFunctionsWithExceptions)
{
    std::vector<std::future<void>> tFutures;
    for (std::size_t i = 0; i < 10; ++i)
    {
        tFutures.emplace_back(ThreadPool::getInstance().add([i]()
                                                            { 
                                        if (i % 2 == 0)
                                        {
                                            throw std::runtime_error("Test exception");
                                        } }));
    }

    for (auto &&future : tFutures)
    {
        if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            ASSERT_THROW(future.get(), std::runtime_error);
        }
    }
}
