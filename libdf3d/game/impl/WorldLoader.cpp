#include "WorldLoader.h"

#include <base/EngineController.h>
#include <game/World.h>
#include <3d/Camera.h>
#include <3d/Light.h>
#include <utils/JsonUtils.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>
#include <render/MaterialLib.h>

namespace df3d { namespace scene_impl {

static void parseFog(const Json::Value &fogNode, World& w)
{
    if (fogNode.empty())
        return;

    auto density = fogNode["density"].asFloat();
    auto color = w.getRenderingParams().getFogColor();
    fogNode["color"] >> color;

    w.getRenderingParams().setFog(density, color);
}

static void parseEntities(const Json::Value &entitiesNode, World &w)
{
    if (entitiesNode.empty())
        return;

    for (auto &entJson : entitiesNode)
        w.spawn(entJson);
}

static void parseAmbientLight(const Json::Value &root, World &w)
{
    auto intensity = utils::json::getOrDefault(root, w.getRenderingParams().getAmbientLight());
    w.getRenderingParams().setAmbientLight(intensity.x, intensity.y, intensity.z);
}

static void parsePostProcessOption(const Json::Value &postFxNode, World &w)
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

    w.getRenderingParams().setPostProcessMaterial(make_shared<Material>(*material));
}

static void parseCamera(const Json::Value &cameraNode, World &w)
{
    if (cameraNode.empty())
        return;

    auto camera = make_shared<Camera>();

    glm::vec3 position, rotation;
    float fov = 60.0f;
    cameraNode["position"] >> position;
    cameraNode["rotation"] >> rotation;
    cameraNode["fov"] >> fov;

    camera->setPosition(position);
    camera->setOrientation(rotation);
    camera->setFov(fov);

    w.setCamera(camera);
}

static void parseLights(const Json::Value &lightsNode, World &w)
{
    for (auto &lightJson : lightsNode)
    {
        Light light(Light::Type::DIRECTIONAL);

        light.setDirection(utils::json::getOrDefault(lightJson["direction"], light.getDirection()));
        light.setDiffuseIntensity(utils::json::getOrDefault(lightJson["diffuse"], light.getDiffuseColor()));
        light.setSpecularIntensity(utils::json::getOrDefault(lightJson["specular"], light.getSpecularColor()));

        w.getRenderingParams().addLight(light);
    }
}

unique_ptr<World> WorldLoader::createWorld(const std::string &resourceFile)
{
    auto res = World::newWorld();

    auto root = utils::json::fromFile(resourceFile);

    parseEntities(root["entities"], *res);
    parseFog(root["fog"], *res);
    parseAmbientLight(root["ambient_light"], *res);
    parsePostProcessOption(root["post_process"], *res);
    parseCamera(root["camera"], *res);
    parseLights(root["lights"], *res);

    return res;
}

} }
