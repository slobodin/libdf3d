#include "MeshComponentSerializer.h"

#include <components/MeshComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

shared_ptr<NodeComponent> MeshComponentSerializer::fromJson(const Json::Value &root)
{
    return make_shared<MeshComponent>(root["path"].asString());
}

Json::Value MeshComponentSerializer::toJson(shared_ptr<NodeComponent> component)
{
    Json::Value result;

    auto comp = static_cast<const MeshComponent*>(component.get());
    result["path"] = "XZ";  // FIXME:

    return result;
}

} }
