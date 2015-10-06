#include "TransformComponentSerializer.h"

#include <components/TransformComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

Component TransformComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<components::TransformComponent>();

    result->setPosition(utils::jsonGetValueWithDefault(root["position"], glm::vec3()));
    result->setOrientation(utils::jsonGetValueWithDefault(root["rotation"], glm::vec3()));
    result->setScale(utils::jsonGetValueWithDefault(root["scale"], glm::vec3(1.0f, 1.0f, 1.0f)));

    return result;
}

Json::Value TransformComponentSerializer::toJson(Component component)
{
    Json::Value result;

    auto comp = (components::TransformComponent*)component.get();

    result["position"] = utils::glmToJson(comp->getPosition());
    result["rotation"] = utils::glmToJson(comp->getRotation());
    result["scale"] = utils::glmToJson(comp->getScale());

    return result;
}

} }