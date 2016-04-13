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
#ifdef _DEBUG
    // Dump some debug info.
    auto aliveCount = 0;
    for (auto &l : m_timeListeners)
        if (l.handle.valid())
            aliveCount++;

    if (aliveCount > 0)
    {
        glog << "TimeManager::~TimeManager total listeners:" << m_timeListeners.size() << logwarn;
        glog << "TimeManager::~TimeManager alive listeners:" << aliveCount << logwarn;
    }
#endif
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

void TimeManager::enqueueForNextUpdate(UpdateFn &&callback)
{
    m_newListeners.push(std::move(callback));
}

void TimeManager::enqueueAction(UpdateFn &&callback, float delay)
{
    Action act;
    act.callback = std::move(callback);
    act.handle.id = ++m_nextHandle.id;
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
            action.handle.invalidate();
        }
    }
}

void TimeManager::cleanStep()
{
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
