#include "df3d_pch.h"
#include "TransformComponentSerializer.h"

#include <components/TransformComponent.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

void load(components::TransformComponent *component, const Json::Value &root)
{
    component->setPosition(utils::jsonGetValueWithDefault(root["position"], glm::vec3()));
    component->setOrientation(utils::jsonGetValueWithDefault(root["rotation"], glm::vec3()));
    component->setScale(utils::jsonGetValueWithDefault(root["scale"], glm::vec3(1.0f, 1.0f, 1.0f)));
}

Json::Value save(components::TransformComponent *component)
{
    Json::Value result;

    result["position"] = utils::glmToJson(component->getPosition());
    result["rotation"] = utils::glmToJson(component->getRotation());
    result["scale"] = utils::glmToJson(component->getScale());

    return result;
}

} } }