#include <ThreadPool.hpp>

std::unique_ptr<ThreadPool> gThreadPool;

ThreadPool &ThreadPool::getInstance()
{
    if (!isCreated())
        createInstance();

    return *gThreadPool;
}

void ThreadPool::createInstance(std::size_t amount)
{
    gThreadPool.reset(new ThreadPool{amount});
}

bool ThreadPool::isCreated() noexcept
{
    return static_cast<bool>(gThreadPool);
}
