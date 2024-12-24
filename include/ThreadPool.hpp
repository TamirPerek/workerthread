#pragma once

#include <thread>
#include <vector>
#include <tuple>
#include <future>
#include <type_traits>
#include <chrono>

class ThreadPool
{
public:
    static ThreadPool &getInstance();

    static void createInstance(std::size_t amount = std::thread::hardware_concurrency());

    static bool isCreated() noexcept;

    ~ThreadPool()
    {
        mFinished = true;
        std::scoped_lock workerLock(mWorkerMutex);
        if (mWorkerfunc.joinable())
            mWorkerfunc.join();

        for(auto &[tThread, tFinished] : mPool)
        {
            if(tThread.joinable())
                tThread.join();
        }
        mPool.clear();
    }

    template <typename T, typename... Args>
    std::future<typename std::invoke_result<T, Args...>::type> add(T &&func, Args&&... args)
    {
        using ReturnType = typename std::invoke_result<T, Args...>::type;
        auto promise = std::make_shared<std::promise<ReturnType>>();
        std::future<ReturnType> future = promise->get_future();

        std::scoped_lock waitingListLock(mWaitingListMutex);

        auto tLambda = [mypromise = std::move(promise), innerFunc = std::forward<T>(func), argsTuple = std::make_tuple(std::forward<Args>(args)...)]() mutable
        {
            try
            {
                if constexpr (std::is_void_v<ReturnType>)
                {
                    std::apply(innerFunc, argsTuple);
                    mypromise->set_value();
                }
                else
                {
                    mypromise->set_value(std::apply(innerFunc, argsTuple));
                }
            }
            catch (...)
            {
                mypromise->set_exception(std::current_exception());
            }
        };

        mWaitingList.emplace(std::move(tLambda));

        return future;
    }

    std::size_t getProcessorCount() const noexcept
    {
        return mProcessorCount;
    }

private:
    ThreadPool() = delete;
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    explicit ThreadPool(std::size_t amount = 0) : mFinished{false}, mProcessorCount{amount}
    {
        for (std::size_t i = 0; i < mProcessorCount; i++)
        {
            mPool.emplace_back(std::thread{}, std::atomic<bool>{true});
        }

        mWorkerfunc = std::thread(&ThreadPool::workerFunc, std::reference_wrapper(*this));
    }

    static void workerFunc(ThreadPool &threadPool) noexcept
    {
        while (!threadPool.mFinished)
        {
            for (auto &[tThread, tFinished] : threadPool.mPool)
            {
                std::scoped_lock workerLock(threadPool.mWorkerMutex);

                if (!tFinished)
                    continue;

                if (tThread.joinable())
                    tThread.join();

                std::scoped_lock tWaitingListLock(threadPool.mWaitingListMutex);

                if (threadPool.mWaitingList.empty())
                    continue;

                auto tNextFunction = threadPool.mWaitingList.front();
                threadPool.mWaitingList.pop();
                tThread = std::thread([&tFlagg = tFinished](std::function<void()> &&xIn)
                                      {
                                            tFlagg = false;
                                            xIn();
                                            tFlagg = true; },
                                      tNextFunction);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }
    }

    std::vector<std::tuple<std::thread, bool>> mPool;
    std::queue<std::function<void()>> mWaitingList;
    std::thread mWorkerfunc;
    std::atomic<bool> mFinished;
    std::size_t mProcessorCount;
    std::mutex mWaitingListMutex;
    std::mutex mWorkerMutex;
};
