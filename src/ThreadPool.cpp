#include <ThreadPool.hpp>

std::unique_ptr<ThreadPool> gThreadPool;

ThreadPool &ThreadPool::getInstance()
{
    if (!isCreated())  
    {
        throw std::runtime_error("ThreadPool is not created");
    }

    return *gThreadPool;
}

void ThreadPool::createInstance(std::size_t amount)
{
    gThreadPool.reset(new ThreadPool{amount});
}

void ThreadPool::removeInstance() noexcept
{
    gThreadPool.reset();
}

bool ThreadPool::isCreated() noexcept
{
    return static_cast<bool>(gThreadPool);
}
