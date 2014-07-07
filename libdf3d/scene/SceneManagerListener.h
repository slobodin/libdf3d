#pragma once

namespace df3d { namespace scene {

class Scene;
class Node;

class SceneManagerListener
{
public:
    SceneManagerListener() { }
    virtual ~SceneManagerListener() { }

    virtual void onNodeAddedToScene(const Node *node) { }
    virtual void onNodeRemovedFromScene() { }
    virtual void onSceneCleared() { }
    virtual void onSceneCreated(const Scene *sc) { }
};

} }
