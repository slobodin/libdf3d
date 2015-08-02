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

class DF3D_DLL NodeComponent : boost::noncopyable
{
    friend class scene::Node;

protected:
    scene::Node *m_holder = nullptr;

public:
    NodeComponent(ComponentType t);
    const ComponentType type;

    virtual ~NodeComponent() { }

    scene::Node *getHolder() { return m_holder; }
    const std::string &getName() const;

    void sendEvent(ComponentEvent ev);

    virtual void onComponentEvent(ComponentEvent ev) { }
    virtual void onNodeEvent(NodeEvent ev) { }
    virtual void onUpdate(float dt) { }
    virtual void onAttached() { }
    virtual void onDetached() { }
    virtual void onDraw(render::RenderQueue *ops) { }
    // FIXME: maybe kind of updatable & renderable components?

    virtual shared_ptr<NodeComponent> clone() const = 0;

    static shared_ptr<NodeComponent> fromJson(const std::string &jsonFile);
    static shared_ptr<NodeComponent> fromJson(const Json::Value &root);
    static Json::Value toJson(shared_ptr<const NodeComponent> component);
};

} }
