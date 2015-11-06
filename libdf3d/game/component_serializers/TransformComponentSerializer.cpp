#include "TransformComponentSerializer.h"

#include <components/TransformComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

shared_ptr<NodeComponent> TransformComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<TransformComponent>();

    result->setPosition(utils::json::getOrDefault(root["position"], glm::vec3()));
    result->setOrientation(utils::json::getOrDefault(root["rotation"], glm::vec3()));
    result->setScale(utils::json::getOrDefault(root["scale"], glm::vec3(1.0f, 1.0f, 1.0f)));

    return result;
}

Json::Value TransformComponentSerializer::toJson(shared_ptr<NodeComponent> component)
{
    Json::Value result;

    auto comp = (TransformComponent*)component.get();

    result["position"] = utils::json::toJson(comp->getPosition());
    result["rotation"] = utils::json::toJson(comp->getRotation());
    result["scale"] = utils::json::toJson(comp->getScale());

    return result;
}

} }