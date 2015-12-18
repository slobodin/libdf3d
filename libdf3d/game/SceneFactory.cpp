#include "SceneFactory.h"

#include <utils/JsonUtils.h>
#include <scene/Node.h>
#include <scene/Camera.h>
#include <components/TransformComponent.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>
#include <render/MaterialLib.h>
#include <game/NodeFactory.h>

namespace df3d {
/*

void parseFog(const Json::Value &fogNode, shared_ptr<Scene> scene)
{
    if (!fogNode)
        return;

    auto density = fogNode["density"].asFloat();
    auto color = scene->getFogColor();
    fogNode["color"] >> color;

    scene->setFog(density, color);
}

Json::Value saveFog(shared_ptr<const Scene> scene)
{
    // TODO:
    return Json::Value();
}

void parseObjects(const Json::Value &objectsNode, shared_ptr<Scene> sc)
{
    if (!objectsNode)
        return;

    for (Json::UInt objIdx = 0; objIdx < objectsNode.size(); ++objIdx)
        sc->addChild(nodeFromJson(objectsNode[objIdx]));
}

Json::Value saveObjects(shared_ptr<const Scene> scene)
{
    Json::Value res(Json::arrayValue);
    for (auto ch : *scene->getRoot())
        res.append(saveNode(ch));

    return res;
}

void parseAmbientLight(const Json::Value &root, shared_ptr<Scene> scene)
{
    auto intensity = utils::json::getOrDefault(root, scene->getAmbientLight());
    scene->setAmbientLight(intensity.x, intensity.y, intensity.z);
}

Json::Value saveAmbientLight(shared_ptr<const Scene> scene)
{
    return utils::json::toJson(scene->getAmbientLight());
}

void parsePostProcessOption(const Json::Value &postFxNode, shared_ptr<Scene> scene)
{
    if (postFxNode.empty())
        return;

    auto mtlLib = postFxNode["materialLib"].asString();
    auto mtlName = postFxNode["materialName"].asString();

    if (mtlLib.empty() || mtlName.empty())
    {
        glog << "Invalid postprocess option. Either materialName or materialLib field is empty." << logwarn;
        return;
    }

    auto materialLibrary = svc().resourceManager().getFactory().createMaterialLib(mtlLib);
    auto material = materialLibrary->getMaterial(mtlName);
    if (!material)
        return;

    scene->setPostProcessMaterial(make_shared<Material>(*material));
}

Json::Value savePostProcessOption(shared_ptr<const Scene> scene)
{
    //auto material = scene->getPostProcessMaterial();
    // TODO:
    return Json::Value();
}

void parseCamera(const Json::Value &cameraNode, shared_ptr<Scene> scene)
{
    auto camera = make_shared<Camera>();

    if (cameraNode.empty())
    {
        // Set up default camera.
        scene->setCamera(camera);
    }
    else
    {
        glm::vec3 position, rotation;
        float fov = 60.0f;
        cameraNode["position"] >> position;
        cameraNode["rotation"] >> rotation;
        cameraNode["fov"] >> fov;

        camera->setPosition(position);
        camera->setOrientation(rotation);
        camera->setFov(fov);
    }

    scene->setCamera(camera);
}

Json::Value saveCamera(shared_ptr<const Scene> scene)
{
    auto cam = scene->getCamera();
    // TODO:
    return Json::Value();
}

shared_ptr<Scene> newScene()
{
    return make_shared<df3d::Scene>();
}

shared_ptr<Scene> sceneFromFile(const std::string &jsonFile)
{
    return sceneFromJson(utils::json::fromFile(jsonFile));
}

shared_ptr<Scene> sceneFromJson(const Json::Value &root)
{
    auto result = newScene();

    parseObjects(root["objects"], result);
    parseFog(root["fog"], result);
    parseAmbientLight(root["ambient_light"], result);
    parsePostProcessOption(root["post_process"], result);
    parseCamera(root["camera"], result);

    return result;
}

Json::Value saveScene(shared_ptr<const Scene> scene)
{
    if (!scene)
    {
        glog << "Failed to serialize null scene" << logwarn;
        return Json::Value();
    }

    Json::Value result;

    result["objects"] = saveObjects(scene);
    result["fog"] = saveFog(scene);
    result["ambient_light"] = saveAmbientLight(scene);
    result["post_process"] = savePostProcessOption(scene);
    result["camera"] = saveCamera(scene);

    return result;
}*/

}
