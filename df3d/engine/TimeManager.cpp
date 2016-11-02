#include "TimeManager.h"

#include <df3d/engine/EngineCVars.h>

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
        {
            auto maxDt = 1.0f / 10.0f;
            dt = std::min(dt, maxDt);
        }

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

TimeManager::TimeSubscriber* TimeManager::findSubscriber(ITimeListener *listener)
{
    for (auto &it : m_timeListeners)
    {
        if (it.valid && (it.listener == listener))
            return &it;
    }

    return nullptr;
}

TimeManager::~TimeManager()
{
#ifdef _DEBUG
    // Dump some debug info.
    auto aliveCount = 0;
    for (auto &l : m_timeListeners)
        if (l.valid)
            aliveCount++;

    if (aliveCount > 0)
    {
        DFLOG_WARN("TimeManager::~TimeManager total listeners: %d", m_timeListeners.size());
        DFLOG_WARN("TimeManager::~TimeManager alive listeners: %d", aliveCount);
    }
#endif
}

void TimeManager::subscribeUpdate(ITimeListener *listener)
{
    if (findSubscriber(listener))
    {
        DFLOG_WARN("Duplicate time manager listener!");
        return;
    }

    m_timeListeners.push_back({ listener, true });
}

void TimeManager::unsubscribeUpdate(ITimeListener *listener)
{
    if (auto subscr = findSubscriber(listener))
        subscr->valid = false;
    else
        DFLOG_WARN("Failed to unsubscribeUpdate");
}

void TimeManager::enqueueForNextUpdate(UpdateFn &&callback)
{
    m_newListeners.push(std::move(callback));
}

void TimeManager::enqueueAction(UpdateFn &&callback, float delay)
{
    Action act;
    act.callback = std::move(callback);
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
        if (listener.valid)
            listener.listener->onUpdate();
    }

    for (auto &action : m_actions)
    {
        action.timeDelay -= dt;

        if (action.timeDelay <= 0.0f)
        {
            action.callback();
            action.timeDelay = 0.0f;
        }
    }
}

void TimeManager::cleanStep()
{
    for (auto it = m_timeListeners.begin(); it != m_timeListeners.end(); )
    {
        if (!it->valid)
            it = m_timeListeners.erase(it);
        else
            it++;
    }

    for (auto it = m_actions.begin(); it != m_actions.end(); )
    {
        if (it->timeDelay <= 0.0f)
            it = m_actions.erase(it);
        else
            it++;
    }
}

}
