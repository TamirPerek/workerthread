#include "ThreadPool.h"

#include <vector>
#include <atomic>
#include <utility>
#include <queue>
#include <mutex>
#include <chrono>

namespace WorkerThread
{
    static std::atomic<bool> gFinished{false};

    struct ThreadPool::Impl
    {
        const unsigned int processor_count{std::thread::hardware_concurrency()};
        std::vector<std::pair<std::thread, bool>> pool;
        std::thread threadPoolWorker;

        std::queue<std::function<void()>> waitingList;

        static void threadPoolWorkerFunc() noexcept;

        std::mutex waitingListMutex;

        Impl() noexcept = default;
        ~Impl() noexcept = default;
        Impl(const Impl &) noexcept = delete;
        Impl(Impl &&) = delete;
        Impl &operator=(const Impl &) noexcept = delete;
        Impl &operator=(Impl &&) = delete;
    };

    ThreadPool::ThreadPool()
    {
        m = std::make_unique<Impl>();

        for (std::size_t i = 0; i < m->processor_count; i++)
        {
            m->pool.emplace_back(std::thread{}, std::atomic<bool>{true});
        }

        m->threadPoolWorker = std::thread(&ThreadPool::Impl::threadPoolWorkerFunc);
    }

    ThreadPool::~ThreadPool()
    {
        gFinished = true;
        m->threadPoolWorker.join();
    }

    ThreadPool &ThreadPool::getInstance() noexcept
    {
        static ThreadPool gInstance;
        return gInstance;
    }

    void ThreadPool::Impl::threadPoolWorkerFunc() noexcept
    {
        while (!gFinished)
        {
            for (auto &[tThread, tFinished] : ThreadPool::getInstance().m->pool)
            {
                if (!tFinished)
                    continue;
                
                if(tThread.joinable())
                    tThread.join();

                std::scoped_lock tWaitingListLock(ThreadPool::getInstance().m->waitingListMutex);

                if (ThreadPool::getInstance().m->waitingList.empty())
                    continue;

                auto tNextFunction = ThreadPool::getInstance().m->waitingList.front();
                ThreadPool::getInstance().m->waitingList.pop();
                tThread = std::thread([&tFlagg = tFinished](std::function<void()> &&xIn)
                                          {
                                            tFlagg = false;
                                            xIn();
                                            tFlagg = true;
                                          },
                                          tNextFunction);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }
    }

    ThreadPool &ThreadPool::AddToPool(std::function<void()> &&xIn) noexcept
    {
        std::scoped_lock tWaitingListLock(m->waitingListMutex);
        m->waitingList.emplace(std::move(xIn));
        return *this;
    }

    ThreadPool &ThreadPool::AddToPool(std::packaged_task<int()> &&xIn) noexcept
    {
        std::scoped_lock tWaitingListLock(m->waitingListMutex);
        // m->waitingList.emplace(std::move(xIn));
        return *this;
    }
}
