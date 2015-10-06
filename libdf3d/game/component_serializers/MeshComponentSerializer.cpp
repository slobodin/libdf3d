#include "MeshComponentSerializer.h"

#include <components/MeshComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

Component MeshComponentSerializer::fromJson(const Json::Value &root)
{
    return make_shared<components::MeshComponent>(root["path"].asString());
}

Json::Value MeshComponentSerializer::toJson(Component component)
{
    Json::Value result;

    auto comp = static_cast<const components::MeshComponent*>(component.get());
    result["path"] = "XZ";  // FIXME:

    return result;
}

} }
