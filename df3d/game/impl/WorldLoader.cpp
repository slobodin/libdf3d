#include "WorldLoader.h"

#include <df3d/engine/EngineController.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/Light.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/engine/resources/ResourceManager.h>

namespace df3d { namespace game_impl {

static void parseFog(const rapidjson::Value &fogNode, World& w)
{
    auto density = fogNode["density"].GetFloat();
    auto color = w.getRenderingParams().getFogColor();

    color = JsonUtils::get(fogNode, "color", color);

    w.getRenderingParams().setFog(density, color);
}

static void parseEntities(const rapidjson::Value &entitiesNode, World &w)
{
    for (const auto &entJson : entitiesNode.GetArray())
        w.spawnFromJson(entJson);
}

static void parsePostProcessOption(const rapidjson::Value &postFxNode, World &w)
{
    DF3D_ASSERT(false);
    /*

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

    w.getRenderingParams().setPostProcessMaterial(make_shared<Material>(*material));*/
}

static void parseCamera(const rapidjson::Value &cameraNode, World &w)
{
    glm::vec3 position, rotation;
    float fov = Camera::DEFAULT_FOV;

    position = JsonUtils::get(cameraNode, "position", position);
    rotation = JsonUtils::get(cameraNode, "rotation", rotation);
    fov = JsonUtils::get(cameraNode, "fov", fov);

    auto camera = make_shared<Camera>(position, fov, Camera::DEFAULT_NEAR_Z, Camera::DEFAULT_FAR_Z);

    camera->setPosition(position);
    camera->setOrientation(rotation);
    camera->setFov(fov);

    w.setCamera(camera);
}

static void parseLights(const rapidjson::Value &lightsNode, World &w)
{
    for (auto &lightJson : lightsNode.GetArray())
    {
        Light light;

        std::string lightName = JsonUtils::get(lightJson, "id", std::string(""));

        auto dir = light.getDirection();
        auto color = light.getColor();
        auto intensity = light.getIntensity();

        dir = JsonUtils::get(lightJson, "direction", dir);
        color = JsonUtils::get(lightJson, "color", color);
        intensity = JsonUtils::get(lightJson, "intensity", intensity);

        light.setDirection(dir);
        light.setColor(color);
        light.setIntensity(intensity);

        w.getRenderingParams().addLight(light, lightName);
    }
}

void WorldLoader::initWorld(const char *resourceFile, World &w)
{
    auto root = JsonUtils::fromFile(resourceFile);
    if (root.IsNull())
        return;

    if (root.HasMember("entities"))
        parseEntities(root["entities"], w);

    if (root.HasMember("fog"))
        parseFog(root["fog"], w);

    auto ambientLight = JsonUtils::get(root, "ambient_light", w.getRenderingParams().getAmbientLight());
    w.getRenderingParams().setAmbientLight(ambientLight.x, ambientLight.y, ambientLight.z);

    if (root.HasMember("post_process"))
        parsePostProcessOption(root["post_process"], w);

    if (root.HasMember("camera"))
        parseCamera(root["camera"], w);

    if (root.HasMember("lights"))
        parseLights(root["lights"], w);
}

} }
