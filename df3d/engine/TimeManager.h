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

class DF3D_DLL Timer
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

DF3D_MAKE_HANDLE(TimeManagerHandle)

class DF3D_DLL TimeManager : NonCopyable
{
public:
    using UpdateFn = std::function<void()>;

private:
    struct TimeSubscriber
    {
        TimeManagerHandle handle;
        UpdateFn callback;
    };

    struct Action
    {
        TimeManagerHandle handle;
        UpdateFn callback;
        float timeDelay;
    };

    std::list<TimeSubscriber> m_timeListeners;
    ConcurrentQueue<UpdateFn> m_pendingListeners;
    ConcurrentQueue<UpdateFn> m_newListeners;
    std::list<Action> m_actions;

    HandleBag<TimeManagerHandle> m_handleBag;

    TimeSubscriber* findSubscriber(TimeManagerHandle handle);

public:
    TimeManager();
    ~TimeManager();

    TimeManagerHandle subscribeUpdate(UpdateFn &&callback);
    void unsubscribeUpdate(TimeManagerHandle handle);

    void enqueueForNextUpdate(UpdateFn &&callback);
    void clearNextUpdateQueue();

    void enqueueAction(UpdateFn &&callback, float delay);

    // Should not be called by client code. TODO: improve encapsulation.
    void update(float dt);
    void cleanStep();
};

}
