#include "AudioComponentSerializer.h"

#include <components/AudioComponent.h>
#include <audio/AudioBuffer.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

shared_ptr<NodeComponent> AudioComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<AudioComponent>(root["path"].asString());

    float pitch = result->getPitch();
    float gain = result->getGain();
    bool looped = result->isLooped();

    root["pitch"] >> pitch;
    root["gain"] >> gain;
    root["looped"] >> looped;

    result->setPitch(pitch);
    result->setGain(gain);
    result->setLooped(looped);

    bool autoPlay = false;
    root["autoplay"] >> autoPlay;
    if (autoPlay)
        result->play();     // FIXME: calling it here.

    return result;
}

Json::Value AudioComponentSerializer::toJson(shared_ptr<NodeComponent> component)
{
    Json::Value result;

    auto comp = static_cast<const AudioComponent*>(component.get());

    result["path"] = comp->getBuffer()->getFilePath();
    result["pitch"] = comp->getPitch();
    result["gain"] = comp->getGain();
    result["looped"] = comp->isLooped();

    return result;
}


} }
