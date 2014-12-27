#include "df3d_pch.h"
#include "TransformComponentSerializer.h"

#include <components/TransformComponent.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

shared_ptr<NodeComponent> TransformComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<TransformComponent>();

    result->setPosition(utils::jsonGetValueWithDefault(root["position"], glm::vec3()));
    result->setOrientation(utils::jsonGetValueWithDefault(root["rotation"], glm::vec3()));
    result->setScale(utils::jsonGetValueWithDefault(root["scale"], glm::vec3(1.0f, 1.0f, 1.0f)));

    return result;
}

Json::Value TransformComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;

    auto comp = (TransformComponent*)component.get();

    result["position"] = utils::glmToJson(comp->getPosition());
    result["rotation"] = utils::glmToJson(comp->getRotation());
    result["scale"] = utils::glmToJson(comp->getScale());

    return result;
}

} } }