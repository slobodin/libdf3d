#include "WorldLoader.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/game/World.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/3d/Light.h>
#include <libdf3d/utils/JsonUtils.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/render/MaterialLib.h>

namespace df3d { namespace game_impl {

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

        std::string lightName;
        lightJson["id"] >> lightName;

        light.setDirection(utils::json::getOrDefault(lightJson["direction"], light.getDirection()));
        light.setDiffuseIntensity(utils::json::getOrDefault(lightJson["diffuse"], light.getDiffuseColor()));
        light.setSpecularIntensity(utils::json::getOrDefault(lightJson["specular"], light.getSpecularColor()));
        light.setName(lightName);

        w.getRenderingParams().addLight(light);
    }
}

void WorldLoader::initWorld(const std::string &resourceFile, World &w)
{
    auto root = utils::json::fromFile(resourceFile);

    parseEntities(root["entities"], w);
    parseFog(root["fog"], w);
    parseAmbientLight(root["ambient_light"], w);
    parsePostProcessOption(root["post_process"], w);
    parseCamera(root["camera"], w);
    parseLights(root["lights"], w);
}

} }
