#include "AudioComponentSerializer.h"

#include <components/AudioComponent.h>
#include <audio/AudioBuffer.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

Component AudioComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<components::AudioComponent>(root["path"].asString());

    result->setPitch(utils::jsonGetValueWithDefault(root["pitch"], result->getPitch()));
    result->setGain(utils::jsonGetValueWithDefault(root["gain"], result->getGain()));
    result->setLooped(utils::jsonGetValueWithDefault(root["looped"], result->isLooped()));

    auto startPlay = utils::jsonGetValueWithDefault(root["autoplay"], false);
    if (startPlay)
        result->play();

    return result;
}

Json::Value AudioComponentSerializer::toJson(Component component)
{
    Json::Value result;

    auto comp = static_cast<const components::AudioComponent*>(component.get());

    result["path"] = comp->getBuffer()->getFilePath();
    result["pitch"] = comp->getPitch();
    result["gain"] = comp->getGain();
    result["looped"] = comp->isLooped();

    return result;
}


} }
