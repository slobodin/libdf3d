#include "NodeFactory.h"

#include <components/ComponentFactory.h>
#include <scene/Node.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace scene {

components::ComponentType componentTypeFromJson(const Json::Value &root)
{
    if (root.empty())
        return components::ComponentType::COUNT;

    if (root["type"].empty())
        return components::ComponentType::COUNT;

    return components::NodeComponent::stringToType(root["type"].asString());
}

shared_ptr<Node> createNode(const std::string &jsonDefinitionFile)
{
    return createNode(utils::jsonLoadFromFile(jsonDefinitionFile));
}

shared_ptr<Node> createNode(const Json::Value &root)
{
    if (root.empty())
    {
        base::glog << "Failed to init scene node from JSON node" << base::logwarn;
        return nullptr;
    }

    auto externalDataJson = root["external_data"];
    if (!externalDataJson.empty())
        return createNode(utils::jsonLoadFromFile(externalDataJson.asString()));

    auto objName = root["name"].asString();
    const auto &componentsJson = root["components"];

    auto result = make_shared<scene::Node>(objName);
    // FIXME: first attach transform, then other components!
    for (const auto &componentJson : componentsJson)
    {
        auto componentType = componentTypeFromJson(componentJson);
        const auto &dataJson = componentJson["data"];
        const auto &externalDataJson = componentJson["external_data"];
        if (dataJson.empty() && externalDataJson.empty())
        {
            base::glog << "Failed to init a component. Empty \"data\" or \"external_data\" field" << base::logwarn;
            return nullptr;
        }

        Node::Component component;
        if (!externalDataJson.empty())
            component = components::create(componentType, utils::jsonLoadFromFile(externalDataJson.asString()));
        else
            component = components::create(componentType, dataJson);

        result->attachComponent(component);
    }

    const auto &childrenJson = root["children"];
    for (Json::UInt objIdx = 0; objIdx < childrenJson.size(); ++objIdx)
    {
        const auto &childJson = childrenJson[objIdx];
        result->addChild(createNode(childJson));
    }

    return result;
}

Json::Value nodeToJson(shared_ptr<const Node> node)
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

} }
