#include "TimeManager.h"

namespace df3d {

void Timer::update()
{
    m_timeElapsed = IntervalBetweenNowAnd(m_timeStarted);
    auto now = std::chrono::system_clock::now();

    for (auto &timeInfo : m_timers)
    {
        timeInfo.prevTime = timeInfo.currTime;
        timeInfo.currTime = now;

        timeInfo.dt = IntervalBetween(timeInfo.currTime, timeInfo.prevTime) * timeInfo.scale;
    }
}

Timer::Timer()
{
    auto now = std::chrono::system_clock::now();

    for (auto &timeInfo : m_timers)
        timeInfo.currTime = timeInfo.prevTime = now;

    m_timeStarted = now;
}

Timer::~Timer()
{

}

TimeManager::TimeSubscriber* TimeManager::findSubscriber(Handle handle)
{
    for (auto &it : m_timeListeners)
    {
        if (it.handle.valid() && (it.handle == handle))
            return &it;
    }

    return nullptr;
}

TimeManager::TimeManager()
{

}

TimeManager::~TimeManager()
{
    // Dump some debug info.
    glog << "TimeManager::~TimeManager total listeners:" << m_timeListeners.size() << logdebug;
    auto aliveCount = 0;
    for (auto &l : m_timeListeners)
        if (l.handle.valid())
            aliveCount++;

    glog << "TimeManager::~TimeManager alive listeners:" << aliveCount << logdebug;
}

TimeManager::Handle TimeManager::subscribeUpdate(const UpdateFn &callback)
{
    TimeSubscriber subscr;
    subscr.callback = callback;
    subscr.handle.id = ++m_nextHandle.id;

    m_timeListeners.push_back(std::move(subscr));

    return m_nextHandle;
}

TimeManager::Handle TimeManager::subscribeUpdate(UpdateFn &&callback)
{
    TimeSubscriber subscr;
    subscr.callback = std::move(callback);
    subscr.handle.id = ++m_nextHandle.id;

    m_timeListeners.push_back(std::move(subscr));

    return m_nextHandle;
}

void TimeManager::unsubscribeUpdate(Handle handle)
{
    auto found = findSubscriber(handle);
    if (found)
        found->handle.id = -1;
    else
        glog << "Trying to remove nonexistent time listener" << loggame;
}

void TimeManager::enqueueForNextUpdate(const UpdateFn &callback)
{
    m_pendingListeners.push(callback);
}

void TimeManager::enqueueForNextUpdate(UpdateFn &&callback)
{
    m_pendingListeners.push(std::move(callback));
}

void TimeManager::update()
{
    // First, fire all pending workers.
    // FIXME: what if paused?
    UpdateFn worker;
    while (m_pendingListeners.tryPop(worker))
        worker();

    // Update client code.
    for (auto &listener : m_timeListeners)
    {
        if (listener.handle.valid())
            listener.callback();
    }
}

void TimeManager::cleanStep()
{
    for (auto it = m_timeListeners.cbegin(); it != m_timeListeners.cend(); )
    {
        if (!it->handle.valid())
            it = m_timeListeners.erase(it);
        else
            it++;
    }

    // TODO_ecs: shrink_to_fit every n sec.
}

}
