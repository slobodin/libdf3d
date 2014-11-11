#pragma once

FWD_MODULE_CLASS(scene, Node)
FWD_MODULE_CLASS(render, RenderQueue)

namespace df3d { namespace components {

enum class ComponentType
{
    TRANSFORM,
    MESH,
    PARTICLE_EFFECT,
    AUDIO,
    PHYSICS,
    LIGHT,
    DEBUG_DRAW,

    COUNT
};

enum class ComponentEvent
{
    TRANFORM_CHANGED,
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
    const char *getName() const;

    void sendEvent(ComponentEvent ev);

    virtual void onEvent(ComponentEvent ev) { }
    virtual void onUpdate(float dt) { }
    virtual void onAttached() { }
    virtual void onDetached() { }
    virtual void onDraw(render::RenderQueue *ops) { }
    // FIXME: maybe kind of updatable & renderable components?

    virtual shared_ptr<NodeComponent> clone() const = 0;

    static shared_ptr<NodeComponent> create(const Json::Value &root);
};

} }