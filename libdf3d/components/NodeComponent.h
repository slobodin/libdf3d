#pragma once

namespace df3d {

class Node;
class RenderQueue;

enum class ComponentType
{
    TRANSFORM,
    MESH,
    PARTICLE_EFFECT,
    AUDIO,
    PHYSICS,
    LIGHT,
    DEBUG_DRAW,
    SPRITE_2D,

    COUNT
};

enum class ComponentEvent
{
    POSITION_CHANGED,
    ORIENTATION_CHANGED,
    SCALE_CHANGED,
    MESH_ASYNC_LOAD_COMPLETE
};

enum class NodeEvent
{
    CHILD_ATTACHED,
    CHILD_REMOVED,
    ALL_CHILDREN_REMOVED
};

class DF3D_DLL NodeComponent : utils::NonCopyable
{
    friend class Node;

protected:
    Node *m_holder = nullptr;

    void sendEvent(ComponentEvent ev);

    NodeComponent(ComponentType t);

public:
    const ComponentType type;

    virtual ~NodeComponent() = default;

    Node* getHolder() { return m_holder; }
    const std::string& getName() const;

    virtual void onComponentEvent(ComponentEvent ev) { }
    virtual void onNodeEvent(NodeEvent ev) { }
    virtual void onAttached() { }
    virtual void onDetached() { }
    virtual void onDraw(RenderQueue *ops) { }
    virtual void onUpdate(float dt) { }

    virtual shared_ptr<NodeComponent> clone() const = 0;

    static const std::string& typeToString(ComponentType type);
    static ComponentType stringToType(const std::string &typeStr);
};

}
