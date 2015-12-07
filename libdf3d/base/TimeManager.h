#pragma once

#include <utils/ConcurrentQueue.h>

namespace df3d {

class TimeListener;

class DF3D_DLL TimeManager : utils::NonCopyable
{
    friend class EngineController;

public:
    using EngineThreadWorker = std::function<void()>;

private:
    struct TimeInfo
    {
        TimePoint prevTime;
        TimePoint currTime;
        float dt = 0.0f;
    };

    TimeInfo m_systemTime;
    TimeInfo m_gameTime;
    TimePoint m_timeStarted;
    float m_timeElapsed = 0.0f;
    float m_gameTimeScale = 1.0f;
    bool m_paused = false;

    struct TimeSubscriber
    {
        bool valid;
        TimeListener *listener;
    };

    // NOTE: using list because no iterators or references are invalidated when push_back'ing.
    // This may happen when registering time listener during update loop (iterating over all listeners).
    std::list<TimeSubscriber> m_timeListeners;
    TimeSubscriber* findSubscriber(TimeListener *l);

    utils::ConcurrentQueue<EngineThreadWorker> m_engineThreadWorkers;

public:
    TimeManager();
    ~TimeManager();

    void registerTimeListener(TimeListener *listener);
    void unregisterTimeListener(TimeListener *listener);

    void setGameTimeScale(float scale);
    void pauseGameTime(bool pause);

    void enqueueForNextUpdate(const EngineThreadWorker &worker);
    void enqueueForNextUpdate(EngineThreadWorker &&worker);

    float getElapsedTime() const { return m_timeElapsed; }

private:
    void updateFrameTime();
    void flushPendingWorkers();
    float getGameFrameTimeDuration();
    float getSystemFrameTimeDuration();
    void updateListeners();
    void cleanInvalidListeners();
};

class DF3D_DLL TimeListener
{
    friend class TimeManager;

    uint32_t m_id = 0;

public:
    virtual void onGameDeltaTime(float dt) { }
    virtual void onSystemDeltaTime(float dt) { }
};

}
