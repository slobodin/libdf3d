#include "LightComponentSerializer.h"

#include <components/LightComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

shared_ptr<NodeComponent> LightComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<LightComponent>(LightComponent::Type::DIRECTIONAL);

    result->setDirection(utils::json::getOrDefault(root["direction"], result->getDirection()));
    result->setDiffuseIntensity(utils::json::getOrDefault(root["diffuse"], result->getDiffuseColor()));
    result->setSpecularIntensity(utils::json::getOrDefault(root["specular"], result->getSpecularColor()));

    return result;
}

Json::Value LightComponentSerializer::toJson(shared_ptr<NodeComponent> component)
{
    Json::Value result;

    auto comp = static_cast<const LightComponent*>(component.get());

    result["type"] = "directional"; // FIXME:
    result["direction"] = utils::json::toJson(comp->getDirection());
    result["diffuse"] = utils::json::toJson(comp->getDiffuseColor());
    result["specular"] = utils::json::toJson(comp->getSpecularColor());

    return result;
}

} }
