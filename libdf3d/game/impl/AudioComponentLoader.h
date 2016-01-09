#pragma once

#include <game/EntityComponentLoader.h>
#include <audio/AudioComponentProcessor.h>

namespace df3d {

class AudioComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        std::string path;
        float pitch = 1.0f, gain = 1.0f;
        bool looped = false;
        bool autoplay = false;

        root["path"] >> path;
        root["pitch"] >> pitch;
        root["gain"] >> gain;
        root["looped"] >> looped;
        root["autoplay"] >> autoplay;

        w.audio().add(e, path);
        w.audio().setPitch(e, pitch);
        w.audio().setGain(e, gain);
        w.audio().setLooped(e, looped);

        if (autoplay)
            w.audio().play(e);
    }
};

}
