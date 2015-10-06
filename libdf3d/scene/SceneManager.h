#pragma once

FWD_MODULE_CLASS(base, EngineController)

namespace df3d { namespace scene {

class Scene;
class Node;
class Camera;
class SceneManagerListener;

class DF3D_DLL SceneManager : utils::NonCopyable
{
    friend class base::EngineController;

    SceneManager();
    ~SceneManager();

    shared_ptr<Scene> m_currentScene;

    bool m_paused = false;

    std::list<shared_ptr<Node>> m_nodesMarkedForRemoval;
    std::list<SceneManagerListener *> m_listeners;

    void update(float dt);
    void cleanStep();

public:
    void clearCurrentScene();
    void pauseSimulation(bool pause);

    bool setCurrentScene(shared_ptr<Scene> scene);
    shared_ptr<Scene> getCurrentScene() const;

    void setCamera(shared_ptr<Camera> camera);
    shared_ptr<Camera> getCamera() const;

    // NOTE:
    // Not immediate removal.
    void removeNodeFromScene(shared_ptr<Node> node);
    void removeNodeFromScene(const std::string &name);

    void addNodeToScene(shared_ptr<Node> node);

    void registerListener(SceneManagerListener *listener);
    void unregisterListener(SceneManagerListener *listener);
};

} }
