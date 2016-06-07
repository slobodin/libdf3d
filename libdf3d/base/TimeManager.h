#pragma once

#include <libdf3d/utils/ConcurrentQueue.h>

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
        TimePoint prevTime;
        TimePoint currTime;
        float scale = 1.0f;

        float dt = 0.0f;
    };

    TimePoint m_timeStarted;
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

class DF3D_DLL TimeManager : utils::NonCopyable
{
public:
    using UpdateFn = std::function<void()>;
    struct Handle
    {
        int64_t id = -1;

        bool valid() const { return id != -1; }
        bool operator== (const Handle &other) const { return id == other.id; }
        void invalidate() { id = -1; }
    };

private:
    struct TimeSubscriber
    {
        Handle handle;
        UpdateFn callback;
    };

    struct Action
    {
        Handle handle;
        UpdateFn callback;
        float timeDelay;
    };

    std::list<TimeSubscriber> m_timeListeners;
    utils::ConcurrentQueue<UpdateFn> m_pendingListeners;
    utils::ConcurrentQueue<UpdateFn> m_newListeners;
    std::list<Action> m_actions;
    Handle m_nextHandle;

    TimeSubscriber* findSubscriber(Handle handle);

public:
    TimeManager();
    ~TimeManager();

    Handle subscribeUpdate(UpdateFn &&callback);
    void unsubscribeUpdate(Handle handle);

    void enqueueForNextUpdate(UpdateFn &&callback);
    void clearNextUpdateQueue();

    void enqueueAction(UpdateFn &&callback, float delay);

    // Should not be called by client code. TODO: improve encapsulation.
    void update(float dt);
    void cleanStep();
};

}
