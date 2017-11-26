#include "WorldLoader.h"

#include <df3d/engine/EngineController.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/Light.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/EntityResource.h>

namespace df3d { namespace game_impl {

static void parseFog(const Json::Value &fogNode, World& w)
{
    auto density = fogNode["density"].asFloat();
    auto color = w.getRenderingParams().getFogColor();

    color = JsonUtils::get(fogNode, "color", color);

    w.getRenderingParams().setFog(density, color);
}

static void parseEntities(const Json::Value &entitiesNode, World &w)
{
    for (const auto &entJson : entitiesNode)
        w.spawnFromJson(entJson);
}

static void parseCamera(const Json::Value &cameraNode, World &w)
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

static void parseLights(const Json::Value &lightsNode, World &w)
{
    for (const auto &lightJson : lightsNode)
    {
        std::string lightName = JsonUtils::get(lightJson, "id", std::string(""));

        Light light(df3d::Id(lightName.c_str()));

        auto dir = light.getDirection();
        auto color = light.getColor();
        auto intensity = light.getIntensity();

        dir = JsonUtils::get(lightJson, "direction", dir);
        color = JsonUtils::get(lightJson, "color", color);
        intensity = JsonUtils::get(lightJson, "intensity", intensity);

        light.setDirection(dir);
        light.setColor(color);
        light.setIntensity(intensity);

        w.getRenderingParams().addLight(light);
    }
    
    DF3D_ASSERT_MESS(w.getRenderingParams().getLights().size() == 2, "FIXME: metal workaround");
}

void WorldLoader::initWorld(const char *resourceFile, World &w)
{
    auto resource = svc().resourceManager().getResource<EntityResource>(Id(resourceFile));
    if (!resource)
    {
        DF3D_ASSERT(false);
        return;
    }

    DF3D_ASSERT(resource->isWorld);

    auto root = resource->root;
    if (root.isNull())
        return;

    if (root.isMember("children"))
        parseEntities(root["children"], w);

    if (root.isMember("fog"))
        parseFog(root["fog"], w);

    auto ambientLight = JsonUtils::get(root, "ambient_light", w.getRenderingParams().getAmbientLight());
    w.getRenderingParams().setAmbientLight(ambientLight.x, ambientLight.y, ambientLight.z);

    if (root.isMember("camera"))
        parseCamera(root["camera"], w);

    if (root.isMember("lights"))
        parseLights(root["lights"], w);
}

} }
