#pragma once

namespace df3d {

class ThreadPool : NonCopyable
{
    friend struct ThreadPoolWorker;

    std::mutex m_mutex;
    std::condition_variable m_condition;

    std::vector<std::thread> m_workers;
    std::deque<std::function<void ()>> m_jobs;
    std::atomic<size_t> m_currentJobs = 0;
    bool m_stop = false;
    size_t m_numWorkers;

public:
    ThreadPool(size_t numWorkers = 2);
    ~ThreadPool();

    void enqueue(const std::function<void ()> &fn);
    size_t getJobsCount() const { return m_currentJobs; }

    void suspend();
    void resume();
};

}