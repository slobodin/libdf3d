#include "AudioComponentSerializer.h"

#include <components/AudioComponent.h>
#include <audio/AudioBuffer.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

shared_ptr<NodeComponent> AudioComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<AudioComponent>(root["path"].asCString());

    result->setPitch(utils::jsonGetValueWithDefault(root["pitch"], result->getPitch()));
    result->setGain(utils::jsonGetValueWithDefault(root["gain"], result->getGain()));
    result->setLooped(utils::jsonGetValueWithDefault(root["looped"], result->isLooped()));
    
    auto startPlay = utils::jsonGetValueWithDefault(root["autoplay"], false);
    if (startPlay)
        result->play();

    return result;
}

Json::Value AudioComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;

    auto comp = static_cast<const AudioComponent*>(component.get());

    result["path"] = comp->getBuffer()->getFilePath();
    result["pitch"] = comp->getPitch();
    result["gain"] = comp->getGain();
    result["looped"] = comp->isLooped();

    return result;
}


} } }