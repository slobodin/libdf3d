#include "df3d_pch.h"
#include "Scene.h"

#include <utils/SceneSerializer.h>
#include <utils/JsonHelpers.h>
#include <scene/Node.h>
#include <render/RenderStats.h>
#include <components/ParticleSystemComponent.h>

namespace df3d { namespace scene {

void statsCollector(render::RenderStats *stats, shared_ptr<Node> n)
{
    stats->totalNodes++;
    //if (n->mesh())
    //    stats->totalNodes++;
    //if (n->vfx())
    //{
    //    stats->totalParticleSystems++;
    //    stats->totalParticles += n->vfx()->getParticlesCount();
    //}
}

Scene::Scene()
    : m_root(make_shared<Node>("__root"))
{
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
    m_postProcessMaterial = material;
}

void Scene::collectStats(render::RenderStats *stats)
{
    //auto fn = [&](shared_ptr<Node> n) 
    //{
    //};

    m_root->traverse(std::bind(statsCollector, stats, std::placeholders::_1));
}

void Scene::collectRenderOperations(render::RenderQueue *ops)
{
    m_root->draw(ops);
}

shared_ptr<Node> Scene::getChildByName(const std::string &name) const
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

void Scene::removeChild(const std::string &name)
{
    m_root->removeChild(name);
}

void Scene::removeAllChildren()
{
    m_root->removeAllChildren();
}

shared_ptr<Scene> Scene::fromJson(const std::string &jsonFile)
{
    return fromJson(utils::jsonLoadFromFile(jsonFile));
}

shared_ptr<Scene> Scene::fromJson(const Json::Value &root)
{
    return utils::serializers::fromJson(root);
}

Json::Value Scene::toJson(shared_ptr<const Scene> scene)
{
    return utils::serializers::toJson(scene);
}

} }
