#pragma once

namespace df3d {

class Scene;
class Node;
class Camera;

class DF3D_DLL SceneManager : utils::NonCopyable
{
    shared_ptr<Scene> m_currentScene;

    bool m_paused = false;

    std::list<shared_ptr<Node>> m_nodesMarkedForRemoval;

public:
    SceneManager();
    ~SceneManager();

    void update(float systemDelta, float gameDelta);
    void cleanStep();

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
};

}
