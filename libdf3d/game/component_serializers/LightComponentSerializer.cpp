#include "LightComponentSerializer.h"

#include <components/LightComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

Component LightComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<components::LightComponent>(components::LightComponent::Type::DIRECTIONAL);

    result->setDirection(utils::jsonGetValueWithDefault(root["direction"], result->getDirection()));
    result->setDiffuseIntensity(utils::jsonGetValueWithDefault(root["diffuse"], result->getDiffuseColor()));
    result->setSpecularIntensity(utils::jsonGetValueWithDefault(root["specular"], result->getSpecularColor()));

    return result;
}

Json::Value LightComponentSerializer::toJson(Component component)
{
    Json::Value result;

    auto comp = static_cast<const components::LightComponent*>(component.get());

    result["type"] = "directional"; // FIXME:
    result["direction"] = utils::glmToJson(comp->getDirection());
    result["diffuse"] = utils::glmToJson(comp->getDiffuseColor());
    result["specular"] = utils::glmToJson(comp->getSpecularColor());

    return result;
}

} }
