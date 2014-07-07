#include "df3d_pch.h"
#include "Scene.h"

#include <utils/SceneLoader.h>
#include <scene/Node.h>

namespace df3d { namespace scene {

Scene::Scene(const char *sceneDefinitionFile)
    : m_root(make_shared<Node>("__root"))
{
    utils::scene_loader::init(this, sceneDefinitionFile);
}

Scene::~Scene()
{
}

void Scene::setAmbientLight(float ra, float ga, float ba)
{
    m_ambientLight.r = boost::algorithm::clamp(ra, 0.0f, 1.0f);
    m_ambientLight.g = boost::algorithm::clamp(ga, 0.0f, 1.0f);
    m_ambientLight.b = boost::algorithm::clamp(ba, 0.0f, 1.0f);
}

glm::vec3 Scene::getAmbientLight() const
{
    return m_ambientLight;
}

void Scene::setFog(float density, const glm::vec3 &color)
{
    m_fogDensity = density;
    m_fogColor = color;
}

void Scene::setFog(float density, float r, float g, float b)
{
    setFog(density, glm::vec3(r, g, b));
}

float Scene::getFogDensity() const
{
    return m_fogDensity;
}

glm::vec3 Scene::getFogColor() const
{
    return m_fogColor;
}

void Scene::setPostProcessMaterial(shared_ptr<render::Material> material)
{
    if (!material)
    {
        base::glog << "Trying to set empty material as post process pass" << base::logwarn;
        return;
    }

    m_postProcessMaterial = material;
}

void Scene::collectRenderOperations(render::RenderQueue *ops)
{
    m_root->draw(ops);
}

shared_ptr<Node> Scene::getChildByName(const char *name) const
{
    return m_root->getChildByName(name);
}

void Scene::addChild(shared_ptr<Node> child)
{
    m_root->addChild(child);
}

void Scene::removeChild(shared_ptr<Node> child)
{
    m_root->removeChild(child);
}

void Scene::removeChild(const char *name)
{
    m_root->removeChild(name);
}

void Scene::removeAllChildren()
{
    m_root->removeAllChildren();
}

} }
