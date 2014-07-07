#pragma once

FWD_MODULE_CLASS(scene, Node)
FWD_MODULE_CLASS(render, RenderQueue)

namespace df3d { namespace components {

enum ComponentType
{
    CT_TRANSFORM,
    CT_MESH,
    CT_PARTICLE_EFFECT,
    CT_AUDIO,
    CT_PHYSICS,
    CT_LIGHT,
    CT_DEBUG_DRAW,

    CT_COUNT
};

enum Event
{
    CE_TRANFORM_CHANGED,
    CE_CHILD_ATTACHED,
    CE_CHILD_REMOVED,
    CE_ALL_CHILDREN_REMOVED
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

    void sendEvent(Event ev);

    virtual void onEvent(Event ev) { }
    virtual void onUpdate(float dt) { }
    virtual void onAttached() { }
    virtual void onDetached() { }
    virtual void onDraw(render::RenderQueue *ops) { }
    // FIXME: maybe kind of updatable & renderable components?

    virtual shared_ptr<NodeComponent> clone() const = 0;

    static shared_ptr<NodeComponent> create(const Json::Value &root);
};

} }