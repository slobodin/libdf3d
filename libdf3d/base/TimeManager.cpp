#include "TimeManager.h"

namespace df3d {

TimeManager::TimeSubscriber* TimeManager::findSubscriber(TimeListener *l)
{
    for (auto &it : m_timeListeners)
    {
        if (it.valid && (it.listener->m_id == l->m_id))
            return &it;
    }

    return nullptr;
}

TimeManager::TimeManager()
{
    auto now = std::chrono::system_clock::now();
    m_systemTime.currTime = m_systemTime.prevTime = now;
    m_gameTime.currTime = m_gameTime.prevTime = now;
    m_timeStarted = now;
}

TimeManager::~TimeManager()
{

}

void TimeManager::registerTimeListener(TimeListener *listener)
{
    static uint32_t id;

    // Can't use just pointer of this object as an id.
    listener->m_id = id++;

    m_timeListeners.push_back({ true, listener });
}

void TimeManager::unregisterTimeListener(TimeListener *listener)
{
    auto found = findSubscriber(listener);
    if (found)
        found->valid = false;
    else
        glog << "Trying to remove nonexistent time listener." << loggame;
}

void TimeManager::setGameTimeScale(float scale)
{
    m_gameTimeScale = scale;
}

void TimeManager::pauseGameTime(bool pause)
{
    m_paused = pause;
}

void TimeManager::enqueueForNextUpdate(const EngineThreadWorker &worker)
{
    m_engineThreadWorkers.push(worker);
}

void TimeManager::enqueueForNextUpdate(EngineThreadWorker &&worker)
{
    m_engineThreadWorkers.push(std::forward<EngineThreadWorker>(worker));
}

void TimeManager::updateFrameTime()
{
    m_timeElapsed = IntervalBetweenNowAnd(m_timeStarted);
    m_systemTime.prevTime = m_systemTime.currTime;
    m_gameTime.prevTime = m_gameTime.currTime;
    m_systemTime.currTime = m_gameTime.currTime = std::chrono::system_clock::now();
    m_systemTime.dt = IntervalBetween(m_systemTime.currTime, m_systemTime.prevTime);
    m_gameTime.dt = IntervalBetween(m_gameTime.currTime, m_gameTime.prevTime);
}

void TimeManager::flushPendingWorkers()
{
    std::function<void()> worker;
    while (m_engineThreadWorkers.tryPop(worker))
        worker();
}

float TimeManager::getGameFrameTimeDuration()
{
    return m_gameTime.dt * m_gameTimeScale;
}

float TimeManager::getSystemFrameTimeDuration()
{
    return m_systemTime.dt;
}

void TimeManager::updateListeners()
{
    auto systemDelta = getSystemFrameTimeDuration();
    auto gameDelta = getGameFrameTimeDuration();

    for (auto listener : m_timeListeners)
        if (listener.valid)
            listener.listener->onSystemDeltaTime(systemDelta);

    if (!m_paused)
    {
        for (auto listener : m_timeListeners)
            if (listener.valid)
                listener.listener->onGameDeltaTime(gameDelta);
    }
}

void TimeManager::cleanInvalidListeners()
{
    for (auto it = m_timeListeners.cbegin(); it != m_timeListeners.cend();)
    {
        if (!it->valid)
            it = m_timeListeners.erase(it);
        else
            it++;
    }
}

}
