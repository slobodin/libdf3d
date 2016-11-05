#include "ThreadPool.h"

namespace df3d {

static const size_t MAX_WORKERS = 4;

struct ThreadPoolWorker
{
    ThreadPool &pool;

    ThreadPoolWorker(ThreadPool &p)
        : pool(p) { }

    void operator()()
    {
        std::function<void ()> job;

        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(pool.m_mutex);
                while (!pool.m_stop && pool.m_jobs.empty())
                    pool.m_condition.wait(lock);

                if (pool.m_stop)
                    return;

                job = pool.m_jobs.front();
                pool.m_jobs.pop_front();
            }

            // Do job.
            job();
            // Clean up job.
            auto fn = std::function<void ()>();
            job.swap(fn);
            DF3D_ASSERT(pool.m_currentJobs > 0);
            --pool.m_currentJobs;
        }
    }
};

ThreadPool::ThreadPool(size_t numWorkers)
    : m_stop(false),
    m_numWorkers(numWorkers)
{
    DF3D_ASSERT(m_numWorkers <= MAX_WORKERS);

    for (size_t i = 0; i < m_numWorkers; i++)
        m_workers.push_back(std::thread(ThreadPoolWorker(*this)));
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }

    m_condition.notify_all();

    for (auto &w : m_workers)
        w.join();
}

void ThreadPool::enqueue(const std::function<void ()> &fn)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_jobs.push_back(fn);
        m_currentJobs++;
    }

    m_condition.notify_one();
}

}
