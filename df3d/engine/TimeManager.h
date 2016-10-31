#pragma once

#include <df3d/lib/containers/ConcurrentQueue.h>
#include <df3d/lib/Utils.h>
#include <df3d/lib/Handles.h>

namespace df3d {

enum TimeChannel
{
    TIME_CHANNEL_SYSTEM,
    TIME_CHANNEL_GAME,

    TIME_CHANNEL_COUNT
};

class Timer
{
    friend class EngineController;

    struct TimeInfo
    {
        TimeUtils::TimePoint prevTime;
        TimeUtils::TimePoint currTime;
        float scale = 1.0f;

        float dt = 0.0f;
    };

    TimeUtils::TimePoint m_timeStarted;
    float m_timeElapsed = 0.0f;

    void update();

    TimeInfo m_timers[TIME_CHANNEL_COUNT];

    TimeInfo& getTimeInfo(TimeChannel channel) { return m_timers[channel]; }
    const TimeInfo& getTimeInfo(TimeChannel channel) const { return m_timers[channel]; }

public:
    Timer();
    ~Timer();

    float getFrameDelta(TimeChannel channel) const { return getTimeInfo(channel).dt; }
    void setScale(TimeChannel channel, float scale) { getTimeInfo(channel).scale = scale; }

    float getElapsedTime() { return m_timeElapsed; }
};

class ITimeListener
{
public:
    virtual void onUpdate() = 0;
};

class TimeManager : NonCopyable
{
public:
    using UpdateFn = std::function<void()>;

private:
    struct TimeSubscriber
    {
        ITimeListener *listener;
        bool valid;
    };

    std::list<TimeSubscriber> m_timeListeners;

    struct Action
    {
        UpdateFn callback;
        float timeDelay;
    };

    std::list<Action> m_actions;

    ConcurrentQueue<UpdateFn> m_pendingListeners;
    ConcurrentQueue<UpdateFn> m_newListeners;

    TimeSubscriber* findSubscriber(ITimeListener *listener);

public:
    TimeManager() = default;
    ~TimeManager();

    void subscribeUpdate(ITimeListener *listener);
    void unsubscribeUpdate(ITimeListener *listener);

    void enqueueForNextUpdate(UpdateFn &&callback);
    void clearNextUpdateQueue();

    void enqueueAction(UpdateFn &&callback, float delay);

    // Should not be called by client code. TODO: improve encapsulation.
    void update(float dt);
    void cleanStep();
};

}
