#pragma once

namespace df3d {

class DF3D_DLL TimeManager : utils::NonCopyable
{
public:
    enum Channel
    {
        SYSTEM_CHANNEL,
        GAME_CHANNEL,

        CHANNELS_COUNT
    };

private:
    TimePoint m_timers[CHANNELS_COUNT];

public:
    TimeManager();
    ~TimeManager();

    //float getFrameDuration()
};

}
