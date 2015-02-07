#include "df3d_pch.h"
#include "SceneSerializer.h"

#include "JsonHelpers.h"
#include <scene/Scene.h>
#include <scene/Node.h>
#include <scene/Camera.h>
#include <scene/SceneManager.h>
#include <components/TransformComponent.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <render/MaterialLib.h>

namespace df3d { namespace utils { namespace serializers {

void parseFog(const Json::Value &fogNode, shared_ptr<scene::Scene> scene)
{
    if (!fogNode)
        return;

    auto density = fogNode["density"].asFloat();
    auto color = jsonGetValueWithDefault(fogNode["color"], scene->getFogColor());

    scene->setFog(density, color);
}

Json::Value saveFog(shared_ptr<const scene::Scene> scene)
{
    // TODO:
    return Json::Value();
}

void parseObjects(const Json::Value &objectsNode, shared_ptr<scene::Scene> scene)
{
    if (!objectsNode)
        return;

    for (Json::UInt objIdx = 0; objIdx < objectsNode.size(); ++objIdx)
        scene->addChild(scene::Node::fromJson(objectsNode[objIdx]));
}

Json::Value saveObjects(shared_ptr<const scene::Scene> scene)
{
    Json::Value res(Json::arrayValue);
    for (auto ch : *scene->getRoot())
        res.append(scene::Node::toJson(ch));

    return res;
}

void parseAmbientLight(const Json::Value &root, shared_ptr<scene::Scene> scene)
{
    auto intensity = jsonGetValueWithDefault(root, scene->getAmbientLight());
    scene->setAmbientLight(intensity.x, intensity.y, intensity.z);
}

Json::Value saveAmbientLight(shared_ptr<const scene::Scene> scene)
{
    return glmToJson(scene->getAmbientLight());
}

void parsePostProcessOption(const Json::Value &postFxNode, shared_ptr<scene::Scene> scene)
{
    if (postFxNode.empty())
        return;

    auto mtlLib = postFxNode["materialLib"].asCString();
    auto mtlName = postFxNode["materialName"].asCString();

    if (!mtlLib || !mtlName)
    {
        base::glog << "Invalid postprocess option. Either materialName or materialLib field is empty." << base::logwarn;
        return;
    }

    auto materialLibrary = g_resourceManager->getResource<render::MaterialLib>(mtlLib);
    auto material = materialLibrary->getMaterial(mtlName);
    if (!material)
        return;

    scene->setPostProcessMaterial(material);
}

Json::Value savePostProcessOption(shared_ptr<const scene::Scene> scene)
{
    auto material = scene->getPostProcessMaterial();
    // TODO:
    return Json::Value();
}

void parseCamera(const Json::Value &cameraNode, shared_ptr<scene::Scene> scene)
{
    auto camera = make_shared<scene::Camera>();

    if (cameraNode.empty())
    {
        // Set up default camera.
        scene->setCamera(camera);
    }
    else
    {
        auto position = jsonGetValueWithDefault(cameraNode["position"], glm::vec3());
        auto rotation = jsonGetValueWithDefault(cameraNode["rotation"], glm::vec3());
        auto fov = jsonGetValueWithDefault(cameraNode["fov"], 60.0f);

        camera->transform()->setPosition(position);
        camera->transform()->setOrientation(rotation);
        camera->setFov(fov);
    }

    scene->setCamera(camera);
}

Json::Value saveCamera(shared_ptr<const scene::Scene> scene)
{
    auto cam = scene->getCamera();
    // TODO:
    return Json::Value();
}

shared_ptr<scene::Scene> fromJson(const Json::Value &root)
{
    auto result = make_shared<scene::Scene>();

    parseObjects(root["objects"], result);
    parseFog(root["fog"], result);
    parseAmbientLight(root["ambient_light"], result);
    parsePostProcessOption(root["post_process"], result);
    parseCamera(root["camera"], result);

    return result;
}

Json::Value toJson(shared_ptr<const scene::Scene> scene)
{
    if (!scene)
    {
        base::glog << "Failed to serialize null scene" << base::logwarn;
        return Json::Value();
    }

    Json::Value result;

    result["objects"] = saveObjects(scene);
    result["fog"] = saveFog(scene);
    result["ambient_light"] = saveAmbientLight(scene);
    result["post_process"] = savePostProcessOption(scene);
    result["camera"] = saveCamera(scene);

    return result;
}

} } }