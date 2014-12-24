#include "df3d_pch.h"
#include "AudioComponentSerializer.h"

#include <components/AudioComponent.h>
#include <audio/AudioBuffer.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

Json::Value save(const components::AudioComponent *component)
{
    Json::Value result;

    result["path"] = component->getBuffer()->getGUID(); // FIXME:
    result["pitch"] = component->getPitch();
    result["gain"] = component->getGain();
    result["looped"] = component->isLooped();

    return result;
}

} } }