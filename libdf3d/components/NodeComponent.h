#pragma once

FWD_MODULE_CLASS(scene, Node)
FWD_MODULE_CLASS(render, RenderQueue)

namespace df3d { namespace components {

enum ComponentType
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
    friend class scene::Node;

protected:
    scene::Node *m_holder = nullptr;

    void sendEvent(ComponentEvent ev);

    NodeComponent(ComponentType t);

public:
    const ComponentType type;

    virtual ~NodeComponent() = default;

    scene::Node* getHolder() { return m_holder; }
    const std::string& getName() const;

    virtual void onComponentEvent(ComponentEvent ev) { }
    virtual void onNodeEvent(NodeEvent ev) { }
    virtual void onAttached() { }
    virtual void onDetached() { }
    virtual void onDraw(render::RenderQueue *ops) { }
    virtual void onUpdate(float dt) { }

    virtual shared_ptr<NodeComponent> clone() const = 0;

    static std::string typeToString(ComponentType type);
    static ComponentType stringToType(const std::string &typeStr);
};

} }
