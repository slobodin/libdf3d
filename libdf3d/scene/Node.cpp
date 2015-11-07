#include "Node.h"

#include <components/TransformComponent.h>
#include <components/MeshComponent.h>
#include <components/LightComponent.h>
#include <components/ParticleSystemComponent.h>
#include <components/AudioComponent.h>
#include <components/PhysicsComponent.h>
#include <components/Sprite2DComponent.h>
#include <utils/Utils.h>

namespace df3d {

static long nodesCount = 0;

bool findNodeByName(const std::string &name, shared_ptr<Node> node)
{
    return node->getName() == name;
}

void Node::broadcastNodeEvent(NodeEvent ev)
{
    for (auto c : m_components)
    {
        if (c)
            c->onNodeEvent(ev);
    }

    for (auto &c : m_children)
        c->broadcastNodeEvent(ev);
}

void Node::broadcastComponentEvent(const NodeComponent *who, ComponentEvent ev)
{
    // Send event to components.
    for (auto c : m_components)
    {
        if (c && c.get() != who)
            c->onComponentEvent(ev);
    }

    // Send event to node also.
    onComponentEvent(who, ev);

    // Send event to children.
    for (auto &c : m_children)
        c->broadcastComponentEvent(who, ev);
}

Node::Node(const std::string &name)
{
    m_nodeName = name;
    if (m_nodeName.empty())
        m_nodeName = std::string("unnamed_node_") + utils::to_string(nodesCount++);

    createComponent<TransformComponent>();
}

Node::~Node()
{

}

void Node::setName(const std::string &newName)
{
    if (m_parent.lock())
    {
        glog << "Can not set name. SHOULD FIX THAT." << logwarn;
        return;
    }

    if (newName.empty())
    {
        glog << "Can not set invalid name for a node." << logwarn;
        return;
    }

    m_nodeName = newName;

    // FIXME:
    // Find in parent, fix parent children.
}

void Node::update(float dt)
{
    m_lifeTime += dt;

    // Update all components.
    for (size_t i = 0; i < (size_t)ComponentType::COUNT; i++)
    {
        auto c = m_components[i];
        if (c)
            c->onUpdate(dt);
    }

    for (auto &child : m_children)
        child->update(dt);
}

void Node::draw(RenderQueue *ops)
{
    if (!isVisible())
        return;

    for (size_t i = 0; i < (size_t)ComponentType::COUNT; i++)
    {
        auto c = m_components[i];
        if (c)
            c->onDraw(ops);
    }

    for (auto &child : m_children)
        child->draw(ops);
}

void Node::traverse(std::function<void (shared_ptr<Node>)> fn)
{
    for (auto &child : m_children)
        child->traverse(fn);

    fn(shared_from_this());
}

void Node::addChild(shared_ptr<Node> child)
{
    if (!child)
    {
        glog << "NULL child append attempt occurred" << logwarn;
        return;
    }

    if (child->m_parent.lock())
    {
        glog << "Node" << child->m_nodeName << "already has a parent" << child->m_parent.lock()->m_nodeName << logwarn;
        return;
    }

    if (getChildByName(child->m_nodeName))
    {
        glog << "Node" << m_nodeName << "already has child with name" << child->m_nodeName << logwarn;
        return;
    }

    child->m_parent = shared_from_this();
    m_children.push_back(child);

    broadcastNodeEvent(NodeEvent::CHILD_ATTACHED);
}

void Node::removeChild(shared_ptr<Node> child)
{
    removeChild(child->getName());
}

void Node::removeChild(const std::string &name)
{
    auto found = std::find_if(m_children.begin(), m_children.end(), std::bind(findNodeByName, name, std::placeholders::_1));
    if (found == m_children.end())
        return;

    m_children.erase(found);

    broadcastNodeEvent(NodeEvent::CHILD_REMOVED);
}

void Node::removeAllChildren()
{
    m_children.clear();

    broadcastNodeEvent(NodeEvent::ALL_CHILDREN_REMOVED);
}

shared_ptr<Node> Node::getChildByName(const std::string &name) const
{
    auto found = std::find_if(m_children.begin(), m_children.end(), std::bind(findNodeByName, name, std::placeholders::_1));
    if (found == m_children.end())
        return nullptr;

    return *found;
}

shared_ptr<Node> Node::clone() const
{
    auto result = shared_ptr<Node>(new Node());
    result->m_visible = m_visible;
    result->m_userPointer = m_userPointer;
    result->m_maxLifeTime = m_maxLifeTime;
    result->m_lifeTime = m_lifeTime;

    // Clone components.
    for (auto c : m_components)
    {
        if (c)
            result->attachComponent(c->clone());
    }

    for (auto child : m_children)
    {
        auto clonedChild = child->clone();
        result->addChild(clonedChild);
    }

    return result;
}

shared_ptr<TransformComponent> Node::transform()
{
    return static_pointer_cast<TransformComponent>(getComponent(ComponentType::TRANSFORM));
}

shared_ptr<MeshComponent> Node::mesh()
{
    return static_pointer_cast<MeshComponent>(getComponent(ComponentType::MESH));
}

shared_ptr<LightComponent> Node::light()
{
    return static_pointer_cast<LightComponent>(getComponent(ComponentType::LIGHT));
}

shared_ptr<AudioComponent> Node::audio()
{
    return static_pointer_cast<AudioComponent>(getComponent(ComponentType::AUDIO));
}

shared_ptr<ParticleSystemComponent> Node::vfx()
{
    return static_pointer_cast<ParticleSystemComponent>(getComponent(ComponentType::PARTICLE_EFFECT));
}

shared_ptr<PhysicsComponent> Node::physics()
{
    return static_pointer_cast<PhysicsComponent>(getComponent(ComponentType::PHYSICS));
}

shared_ptr<Sprite2DComponent> Node::sprite2d()
{
    return static_pointer_cast<Sprite2DComponent>(getComponent(ComponentType::SPRITE_2D));
}

size_t Node::attachedComponentsCount() const
{
    size_t result = 0;
    for (auto component : m_components)
    {
        if (component)
            result++;
    }

    return result;
}

void Node::attachComponent(shared_ptr<NodeComponent> component)
{
    if (!component)
    {
        glog << "Failed to attach a null component to a node" << logwarn;
        return;
    }

    auto idx = (size_t)component->type;

    auto currentComponent = m_components[idx];
    if (currentComponent)
    {
        currentComponent->onDetached();
        currentComponent->m_holder = nullptr;
    }

    component->m_holder = this;     // shared_from_this(), wanna use weak_ptr, but doesn't work in ctor.
    m_components[idx] = component;

    component->onAttached();
}

void Node::detachComponent(ComponentType type)
{
    auto component = m_components[(size_t)type];
    if (!component)
    {
        glog << "Trying to detach non existing node component" << logwarn;
        return;
    }

    component->onDetached();
    m_components[(size_t)type].reset();
}

}
