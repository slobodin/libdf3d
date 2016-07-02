#include "TimeManager.h"

namespace df3d {

void Timer::update()
{
    m_timeElapsed = TimeUtils::IntervalBetweenNowAnd(m_timeStarted);
    auto now = std::chrono::system_clock::now();

    for (size_t i = 0; i < TIME_CHANNEL_COUNT; i++)
    {
        auto &timeInfo = m_timers[i];

        timeInfo.prevTime = timeInfo.currTime;
        timeInfo.currTime = now;

        auto dt = TimeUtils::IntervalBetween(timeInfo.currTime, timeInfo.prevTime);
        if (i == TIME_CHANNEL_GAME)
            dt = std::min(dt, 1.0f);

        timeInfo.dt = dt * timeInfo.scale;
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

TimeManager::TimeSubscriber* TimeManager::findSubscriber(TimeManagerHandle handle)
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
#ifdef _DEBUG
    // Dump some debug info.
    auto aliveCount = 0;
    for (auto &l : m_timeListeners)
        if (l.handle.valid())
            aliveCount++;

    if (aliveCount > 0)
    {
        DFLOG_WARN("TimeManager::~TimeManager total listeners: %d", m_timeListeners.size());
        DFLOG_WARN("TimeManager::~TimeManager alive listeners: %d", aliveCount);
    }
#endif
}

TimeManagerHandle TimeManager::subscribeUpdate(UpdateFn &&callback)
{
    TimeSubscriber subscr;
    subscr.callback = std::move(callback);
    subscr.handle = m_handleBag.getNew();

    m_timeListeners.push_back(std::move(subscr));

    return subscr.handle;
}

void TimeManager::unsubscribeUpdate(TimeManagerHandle handle)
{
    auto found = findSubscriber(handle);
    if (found)
    {
        m_handleBag.release(handle);
        found->handle.invalidate();
    }
    else
        DFLOG_WARN("Trying to remove nonexistent time listener");
}

void TimeManager::enqueueForNextUpdate(UpdateFn &&callback)
{
    m_newListeners.push(std::move(callback));
}

void TimeManager::clearNextUpdateQueue()
{
    m_newListeners.clear();
    m_pendingListeners.clear();
}

void TimeManager::enqueueAction(UpdateFn &&callback, float delay)
{
    Action act;
    act.callback = std::move(callback);
    act.handle = m_handleBag.getNew();
    act.timeDelay = delay;

    m_actions.push_back(std::move(act));
}

void TimeManager::update(float dt)
{
    // First, fire all pending workers.
    UpdateFn worker;
    while (m_pendingListeners.tryPop(worker))
        worker();

    while (m_newListeners.tryPop(worker))
        m_pendingListeners.push(std::move(worker));

    // Update client code.
    for (auto &listener : m_timeListeners)
    {
        if (listener.handle.valid())
            listener.callback();
    }

    for (auto &action : m_actions)
    {
        action.timeDelay -= dt;

        if (action.timeDelay <= 0.0f)
        {
            action.callback();
            m_handleBag.release(action.handle);
            action.handle.invalidate();
        }
    }
}

void TimeManager::cleanStep()
{
    m_handleBag.cleanup();

    for (auto it = m_timeListeners.begin(); it != m_timeListeners.end(); )
    {
        if (!it->handle.valid())
            it = m_timeListeners.erase(it);
        else
            it++;
    }

    for (auto it = m_actions.begin(); it != m_actions.end(); )
    {
        if (!it->handle.valid())
            it = m_actions.erase(it);
        else
            it++;
    }

    // TODO_ecs: shrink_to_fit every n sec.
}

}
