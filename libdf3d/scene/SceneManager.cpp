#include "df3d_pch.h"
#include "SceneManager.h"

#include "Scene.h"
#include "Node.h"
#include "Camera.h"
#include "SceneManagerListener.h"
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>
#include <utils/Utils.h>

namespace df3d { namespace scene {

SceneManager::SceneManager()
{
    base::glog << "Initializing scene manager" << base::logmess;
}

SceneManager::~SceneManager()
{

}

void SceneManager::update(float dt)
{
    if (!m_currentScene || m_paused)
        return;

    m_currentScene->getRoot()->update(dt);
}

void SceneManager::cleanStep()
{
    for (auto it : m_nodesMarkedForRemoval)
    {
        m_currentScene->removeChild(it);

        for (auto listener : m_listeners)
            listener->onNodeRemovedFromScene();
    }

    m_nodesMarkedForRemoval.clear();

    std::list<shared_ptr<Node>> expiredNodes;

    m_currentScene->getRoot()->traverse([&expiredNodes](shared_ptr<Node> node)
    {
        auto maxLifeTime = node->getMaxLifeTime();
        if (maxLifeTime > 0 && node->getLifeTime() > maxLifeTime)
        {
            expiredNodes.push_back(node);
        }
    });

    // Post order traversal. Can remove.
    for (auto node : expiredNodes)
    {
        node->getParent()->removeChild(node);
    }
}

void SceneManager::clearCurrentScene()
{
    if (!m_currentScene)
        return;

    m_currentScene.reset();
    
    m_nodesMarkedForRemoval.clear();
    m_currentSceneName.clear();

    g_resourceManager->unloadUnused();

    m_paused = false;

    for (auto listener : m_listeners)
        listener->onSceneCleared();
}

void SceneManager::pauseSimulation(bool pause)
{
    m_paused = pause;
}

shared_ptr<Scene> SceneManager::setCurrentScene(const char *filePath)
{
    auto sceneId = resources::createGUIDFromPath(filePath);

    if (sceneId == m_currentSceneName)
    {
        base::glog << "Scene with name" << filePath << "already set" << base::logwarn;
        return nullptr;
    }

    clearCurrentScene();

    auto scene = scene::Scene::fromJson(filePath);
    if (!scene)
    {
        base::glog << "Can not set scene to scene manager. Node at" << filePath << "is invalid" << base::logwarn;
        return nullptr;
    }

    m_currentScene = scene;
    m_currentSceneName = sceneId;

    for (auto listener : m_listeners)
        listener->onSceneCreated(m_currentScene.get());

    base::glog << "Scene manager was set up for" << filePath << base::logdebug;

    return m_currentScene;
}

shared_ptr<Scene> SceneManager::getCurrentScene() const
{
    return m_currentScene;
}

void SceneManager::setCamera(shared_ptr<Camera> camera)
{
    if (m_currentScene)
        m_currentScene->m_camera = camera;
    else
        base::glog << "Can not set camera to an empty scene" << base::logwarn;
}

shared_ptr<Camera> SceneManager::getCamera() const
{
    return m_currentScene ? m_currentScene->m_camera : nullptr;
}

void SceneManager::removeNodeFromScene(shared_ptr<Node> node)
{
    m_nodesMarkedForRemoval.push_back(node);
}

void SceneManager::removeNodeFromScene(const char *name)
{
    // TODO:
    // 
    assert(false);
}

void SceneManager::addNodeToScene(shared_ptr<Node> node)
{
    m_currentScene->addChild(node);

    for (auto listener : m_listeners)
        listener->onNodeAddedToScene(node.get());
}

void SceneManager::registerListener(SceneManagerListener *listener)
{
    if (utils::contains(m_listeners, listener))
    {
        base::glog << "Trying to add duplicate scene manager listener" << base::logwarn;
        return;
    }

    m_listeners.push_back(listener);
}

void SceneManager::unregisterListener(SceneManagerListener *listener)
{
    auto found = std::find(m_listeners.cbegin(), m_listeners.cend(), listener);

    if (found == m_listeners.cend())
    {
        base::glog << "Trying to remove not existing scene manager listener" << base::logwarn;
        return;
    }

    m_listeners.erase(found);
}

} }