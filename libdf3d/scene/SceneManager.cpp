#include "SceneManager.h"

#include "Scene.h"
#include "Node.h"
#include "Camera.h"
#include "SceneManagerListener.h"
#include <base/Service.h>
#include <resources/FileDataSource.h>
#include <resources/Resource.h>
#include <utils/Utils.h>

namespace df3d { namespace scene {

SceneManager::SceneManager()
{
    glog << "Initializing scene manager" << base::logmess;
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

    if (m_currentScene)
    {
        std::list<shared_ptr<Node>> expiredNodes;

        m_currentScene->getRoot()->traverse([&expiredNodes](shared_ptr<Node> node)
        {
            auto maxLifeTime = node->getMaxLifeTime();
            if (maxLifeTime > 0 && node->getLifeTime() > maxLifeTime)
                expiredNodes.push_back(node);
        });

        // Post order traversal. Can remove.
        for (auto node : expiredNodes)
            node->getParent()->removeChild(node);
    }
}

void SceneManager::clearCurrentScene()
{
    if (!m_currentScene)
        return;

    m_currentScene.reset();
    m_nodesMarkedForRemoval.clear();

    svc().resourceMgr.unloadUnused();

    m_paused = false;

    for (auto listener : m_listeners)
        listener->onSceneCleared();
}

void SceneManager::pauseSimulation(bool pause)
{
    m_paused = pause;
}

bool SceneManager::setCurrentScene(shared_ptr<Scene> scene)
{
    if (!scene)
    {
        df3d::glog << "Failed to set up scene manager with null scene" << df3d::base::logwarn;
        return false;
    }

    clearCurrentScene();

    m_currentScene = scene;

    for (auto listener : m_listeners)
        listener->onSceneCreated(m_currentScene.get());

    glog << "Scene manager was set up for a new scene" << base::logdebug;

    return true;
}

shared_ptr<Scene> SceneManager::getCurrentScene() const
{
    return m_currentScene;
}

void SceneManager::setCamera(shared_ptr<Camera> camera)
{
    if (m_currentScene)
        m_currentScene->setCamera(camera);
    else
        glog << "Can not set camera to an empty scene" << base::logwarn;
}

shared_ptr<Camera> SceneManager::getCamera() const
{
    return m_currentScene ? m_currentScene->getCamera() : nullptr;
}

void SceneManager::removeNodeFromScene(shared_ptr<Node> node)
{
    m_nodesMarkedForRemoval.push_back(node);
}

void SceneManager::removeNodeFromScene(const std::string &name)
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
        glog << "Trying to add duplicate scene manager listener" << base::logwarn;
        return;
    }

    m_listeners.push_back(listener);
}

void SceneManager::unregisterListener(SceneManagerListener *listener)
{
    auto found = std::find(m_listeners.cbegin(), m_listeners.cend(), listener);

    if (found == m_listeners.cend())
    {
        glog << "Trying to remove not existing scene manager listener" << base::logwarn;
        return;
    }

    m_listeners.erase(found);
}

} }
