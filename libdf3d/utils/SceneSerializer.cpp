#include "df3d_pch.h"
#include "SceneSerializer.h"

#include "JsonHelpers.h"
#include <scene/Scene.h>
#include <scene/Node.h>
#include <scene/Camera.h>
#include <scene/SceneManager.h>
#include <components/TransformComponent.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <render/MaterialLib.h>

namespace df3d { namespace utils { namespace serializers {

void parseFog(const Json::Value &fogNode, scene::Scene *sc)
{
    if (!fogNode)
        return;

    auto density = fogNode["density"].asFloat();
    auto color = jsonGetValueWithDefault(fogNode["color"], sc->getFogColor());

    sc->setFog(density, color);
}

Json::Value saveFog(const scene::Scene *sc)
{
    // TODO:
    return Json::Value();
}

void parseObjects(const Json::Value &objectsNode, scene::Scene *sc)
{
    if (!objectsNode)
        return;

    for (Json::UInt objIdx = 0; objIdx < objectsNode.size(); ++objIdx)
        sc->addChild(scene::Node::fromJson(objectsNode[objIdx]));
}

Json::Value saveObjects(const scene::Scene *sc)
{
    Json::Value res(Json::arrayValue);
    for (auto ch : *sc->getRoot())
        res.append(scene::Node::toJson(ch.second));

    return res;
}

void parseAmbientLight(const Json::Value &root, scene::Scene *sc)
{
    auto intensity = jsonGetValueWithDefault(root, sc->getAmbientLight());
    sc->setAmbientLight(intensity.x, intensity.y, intensity.z);
}

Json::Value saveAmbientLight(const scene::Scene *sc)
{
    auto ambientIntensity = sc->getAmbientLight();
    // TODO:
    return Json::Value();
}

void parsePostProcessOption(const Json::Value &postFxNode, scene::Scene *sc)
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

    sc->setPostProcessMaterial(material);
}

Json::Value savePostProcessOption(const scene::Scene *sc)
{
    auto material = sc->getPostProcessMaterial();
    // TODO:
    return Json::Value();
}

void parseCamera(const Json::Value &cameraNode, scene::Scene *sc)
{
    if (cameraNode.empty())
    {
        // Set up default camera.
        sc->setCamera(make_shared<scene::Camera>());
        return;
    }

    auto type = cameraNode["type"].asString();
    auto position = jsonGetValueWithDefault(cameraNode["position"], glm::vec3());
    auto rotation = jsonGetValueWithDefault(cameraNode["rotation"], glm::vec3());
    auto freeMove = jsonGetValueWithDefault(cameraNode["free_move"], false);
    auto fov = jsonGetValueWithDefault(cameraNode["fov"], 60.0f);
    auto velocity = jsonGetValueWithDefault(cameraNode["velocity"], 0.0f);

    shared_ptr<scene::Camera> camera = nullptr;
    if (type.empty() || type == "Camera")
    {
        camera = make_shared<scene::Camera>();
    }
    //else if (type == "FPSCamera")
    //{
    //    auto fpscamera = make_shared<scene::FPSCamera>(velocity);
    //    fpscamera->setFreeMove(freeMove);

    //    camera = fpscamera;
    //}
    else
    {
        base::glog << "Unknown camera type found while parsing scene definition" << type << base::logwarn;
        return;
    }

    camera->transform()->setPosition(position);
    camera->transform()->setOrientation(rotation);
    camera->setFov(fov);

    sc->setCamera(camera);
}

Json::Value saveCamera(const scene::Scene *sc)
{
    auto cam = sc->getCamera();
    // TODO:
    return Json::Value();
}

void load(scene::Scene *sc, const char *sceneDefinitionFile)
{
    auto root = jsonLoadFromFile(sceneDefinitionFile);
    if (root.empty())
        return;

    parseObjects(root["objects"], sc);
    parseFog(root["fog"], sc);
    parseAmbientLight(root["ambient_light"], sc);
    parsePostProcessOption(root["post_process"], sc);
    parseCamera(root["camera"], sc);
}

void save(const scene::Scene *sc, const char *sceneDefinitionFile)
{
    if (!sc)
    {
        base::glog << "Failed to serialize null scene" << base::logwarn;
        return;
    }

    std::ofstream file(sceneDefinitionFile);    // This function intended to be used only on desktop, so use default filestream.
    if (!file)
    {
        base::glog << "Failed to open file" << sceneDefinitionFile << ". Can not serialize scene" << base::logwarn;
        return;
    }

    Json::StyledWriter writer;
    Json::Value root;

    root["objects"] = saveObjects(sc);
    root["fog"] = saveFog(sc);
    root["ambient_light"] = saveAmbientLight(sc);
    root["post_process"] = savePostProcessOption(sc);
    root["camera"] = saveCamera(sc);

    file << writer.write(root);

    base::glog << "Scene was successfully saved to" << sceneDefinitionFile << base::logdebug;
}

} } }