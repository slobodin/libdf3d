#include "WorldLoader.h"

#include <df3d/engine/EngineController.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/Light.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/engine/render/MaterialLib.h>

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
    auto intensity = JsonUtils::getOrDefault(root, w.getRenderingParams().getAmbientLight());
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
        DFLOG_WARN("Invalid postprocess option. Either materialName or materialLib field is empty.");
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

    glm::vec3 position, rotation;
    float fov = Camera::DEFAULT_FOV;
    cameraNode["position"] >> position;
    cameraNode["rotation"] >> rotation;
    cameraNode["fov"] >> fov;

    auto camera = make_shared<Camera>(position, fov, Camera::DEFAULT_NEAR_Z, Camera::DEFAULT_FAR_Z);

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

        light.setDirection(JsonUtils::getOrDefault(lightJson["direction"], light.getDirection()));
        light.setDiffuseIntensity(JsonUtils::getOrDefault(lightJson["diffuse"], glm::vec3(light.getDiffuseColor())));
        light.setSpecularIntensity(JsonUtils::getOrDefault(lightJson["specular"], glm::vec3(light.getSpecularColor())));
        light.setName(lightName);

        w.getRenderingParams().addLight(light);
    }
}

void WorldLoader::initWorld(const std::string &resourceFile, World &w)
{
    auto root = JsonUtils::fromFile(resourceFile);

    parseEntities(root["entities"], w);
    parseFog(root["fog"], w);
    parseAmbientLight(root["ambient_light"], w);
    parsePostProcessOption(root["post_process"], w);
    parseCamera(root["camera"], w);
    parseLights(root["lights"], w);
}

} }
