#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <future>

namespace WorkerThread
{
    class ThreadPool
    {
    private:
        ThreadPool();
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool &operator=(const ThreadPool &) = delete;
        ThreadPool &operator=(ThreadPool &&) = delete;

        struct Impl;
        std::unique_ptr<Impl> m;

    public:
        static ThreadPool &getInstance() noexcept;
        ~ThreadPool();

        ThreadPool &AddToPool(std::function<void()> &&) noexcept;
        ThreadPool &AddToPool(std::packaged_task<int()> &&) noexcept;
    };

}