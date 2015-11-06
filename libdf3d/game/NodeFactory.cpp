#include "NodeFactory.h"

#include "ComponentFactory.h"
#include <scene/Node.h>
#include <utils/JsonUtils.h>

namespace df3d {

ComponentType componentTypeFromJson(const Json::Value &root)
{
    if (root.empty())
        return ComponentType::COUNT;

    if (root["type"].empty())
        return ComponentType::COUNT;

    return NodeComponent::stringToType(root["type"].asString());
}

SceneNode newNode(const std::string &name)
{
    return make_shared<Node>(name);
}

SceneNode nodeFromFile(const std::string &jsonDefinitionFile)
{
    return nodeFromJson(utils::json::fromFile(jsonDefinitionFile));
}

SceneNode nodeFromJson(const Json::Value &root)
{
    if (root.empty())
    {
        glog << "Failed to init scene node from JSON node" << logwarn;
        return nullptr;
    }

    auto externalDataJson = root["external_data"];
    if (!externalDataJson.empty())
        return nodeFromFile(externalDataJson.asString());

    auto objName = root["name"].asString();
    const auto &componentsJson = root["components"];

    auto result = make_shared<Node>(objName);
    // FIXME: first attach transform, then other components!
    for (const auto &componentJson : componentsJson)
    {
        auto componentType = componentTypeFromJson(componentJson);
        const auto &dataJson = componentJson["data"];
        const auto &externalDataJson = componentJson["external_data"];
        if (dataJson.empty() && externalDataJson.empty())
        {
            glog << "Failed to init a component. Empty \"data\" or \"external_data\" field" << logwarn;
            return nullptr;
        }

        shared_ptr<NodeComponent> component;
        if (!externalDataJson.empty())
            component = componentFromFile(componentType, externalDataJson.asString());
        else
            component = componentFromJson(componentType, dataJson);

        result->attachComponent(component);
    }

    const auto &childrenJson = root["children"];
    for (Json::UInt objIdx = 0; objIdx < childrenJson.size(); ++objIdx)
    {
        const auto &childJson = childrenJson[objIdx];
        result->addChild(nodeFromJson(childJson));
    }

    return result;
}

Json::Value saveNode(SceneNode node)
{
    assert(false && "TODO: implement serializing");

    return Json::Value();
    /*
    Json::Value result(Json::objectValue);
    Json::Value componentsJson(Json::arrayValue);
    Json::Value childrenJson(Json::arrayValue);

    result["name"] = node->getName();

    for (size_t i = 0; i < components::COUNT; i++)
    {
        auto comp = node->getComponent(static_cast<components::ComponentType>(i));
        if (!comp)
            continue;

        componentsJson.append(components::NodeComponent::toJson(comp));
    }

    for (auto it = node->cbegin(); it != node->cend(); it++)
        childrenJson.append(toJson(*it));

    result["components"] = componentsJson;
    result["children"] = childrenJson;

    return result;
    */
}

}
