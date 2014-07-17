#include "df3d_pch.h"
#include "SceneLoader.h"

#include "JsonHelpers.h"
#include <scene/Scene.h>
#include <scene/Node.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <render/MaterialLib.h>

namespace df3d { namespace utils { namespace scene_loader {

void parseFog(const Json::Value &fogNode, scene::Scene *sc)
{
    if (!fogNode)
        return;

    auto density = fogNode["density"].asDouble();
    auto color = jsonGetValueWithDefault(fogNode["color"], sc->getFogColor());

    sc->setFog((float)density, color);
}

void parseObjects(const Json::Value &objectsNode, scene::Scene *sc)
{
    if (!objectsNode)
        return;

    for (Json::UInt objIdx = 0; objIdx < objectsNode.size(); ++objIdx)
        sc->addChild(scene::Node::fromJson(objectsNode[objIdx]));
}

void parseAmbientLight(const Json::Value &root, scene::Scene *sc)
{
    auto intensity = jsonGetValueWithDefault(root, sc->getAmbientLight());
    sc->setAmbientLight(intensity.x, intensity.y, intensity.z);
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

void init(scene::Scene *sc, const char *sceneDefinitionFile)
{
    auto root = jsonLoadFromFile(sceneDefinitionFile);
    if (root.empty())
        return;

    parseObjects(root["objects"], sc);
    parseFog(root["fog"], sc);
    parseAmbientLight(root["ambient_light"], sc);
    parsePostProcessOption(root["post_process"], sc);
}

} } }