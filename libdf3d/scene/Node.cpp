#include "df3d_pch.h"
#include "Node.h"

#include <components/TransformComponent.h>
#include <components/MeshComponent.h>
#include <components/LightComponent.h>
#include <components/ParticleSystemComponent.h>
#include <components/AudioComponent.h>
#include <components/PhysicsComponent.h>
#include <components/ScriptComponent.h>
#include <utils/Utils.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace scene {

static long nodesCount = 0;

bool findNodeByName(const std::string &name, shared_ptr<Node> node)
{
    return node->getName() == name;
}

void Node::broadcastNodeEvent(components::ComponentEvent ev)
{
    for (auto c : m_components)
    {
        if (c)
            c->onEvent(ev);
    }

    for (auto &c : m_children)
        c->broadcastNodeEvent(ev);
}

void Node::broadcastComponentEvent(const components::NodeComponent *who, components::ComponentEvent ev)
{
    for (auto c : m_components)
    {
        if (c && c.get() != who)
            c->onEvent(ev);
    }

    onComponentEvent(who, ev);

    for (auto &c : m_children)
        c->broadcastComponentEvent(who, ev);
}

Node::Node(const char *name)
{
    m_nodeName = name;
    if (m_nodeName.empty())
        m_nodeName = std::string("unnamed_node_") + std::to_string(nodesCount++);

    // NOTE: does node have to have transform by default?
    // Has transform by default.
    //attachComponent(make_shared<components::TransformComponent>());
}

Node::~Node()
{

}

void Node::setName(const char *newName)
{
    if (m_parent.lock())
    {
        base::glog << "Can not set name. SHOULD FIX THAT." << base::logwarn;
        return;
    }

    if (!newName)
    {
        base::glog << "Can not set invalid name for a node." << base::logwarn;
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
    for (size_t i = 0; i < components::COUNT; i++)
    {
        auto c = m_components[i];
        if (c)
            c->onUpdate(dt);
    }

    for (auto &child : m_children)
        child->update(dt);
}

void Node::draw(render::RenderQueue *ops)
{
    if (!isVisible())
        return;

    for (size_t i = 0; i < components::COUNT; i++)
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
        base::glog << "NULL child append attempt occurred" << base::logwarn;
        return;
    }

    if (child->m_parent.lock())
    {
        base::glog << "Node" << child->m_nodeName << "already has a parent" << child->m_parent.lock()->m_nodeName << base::logwarn;
        return;
    }

    if (getChildByName(child->m_nodeName.c_str()))
    {
        base::glog << "Node" << m_nodeName << "already has child with name" << child->m_nodeName << base::logwarn;
        return;
    }

    child->m_parent = shared_from_this();
    m_children.push_back(child);

    broadcastNodeEvent(components::ComponentEvent::CHILD_ATTACHED);
}

void Node::removeChild(shared_ptr<Node> child)
{
    removeChild(child->getName().c_str());
}

void Node::removeChild(const char *name)
{
    auto found = std::find_if(m_children.begin(), m_children.end(), std::bind(findNodeByName, name, std::placeholders::_1));
    if (found == m_children.end())
        return;

    m_children.erase(found);

    broadcastNodeEvent(components::ComponentEvent::CHILD_REMOVED);
}

void Node::removeAllChildren()
{
    m_children.clear();

    broadcastNodeEvent(components::ComponentEvent::ALL_CHILDREN_REMOVED);
}

shared_ptr<Node> Node::getChildByName(const char *name) const
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

shared_ptr<components::TransformComponent> Node::transform()
{ 
    return static_pointer_cast<components::TransformComponent>(getComponent(components::TRANSFORM));
}

shared_ptr<components::MeshComponent> Node::mesh()
{ 
    return static_pointer_cast<components::MeshComponent>(getComponent(components::MESH));
}

shared_ptr<components::LightComponent> Node::light()
{
    return static_pointer_cast<components::LightComponent>(getComponent(components::LIGHT));
}

shared_ptr<components::AudioComponent> Node::audio()
{
    return static_pointer_cast<components::AudioComponent>(getComponent(components::AUDIO));
}

shared_ptr<components::ParticleSystemComponent> Node::vfx()
{
    return static_pointer_cast<components::ParticleSystemComponent>(getComponent(components::PARTICLE_EFFECT));
}

shared_ptr<components::PhysicsComponent> Node::physics()
{
    return static_pointer_cast<components::PhysicsComponent>(getComponent(components::PHYSICS));
}

shared_ptr<components::ScriptComponent> Node::script()
{
    return static_pointer_cast<components::ScriptComponent>(getComponent(components::SCRIPT));
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

void Node::attachComponent(shared_ptr<components::NodeComponent> component)
{
    if (!component)
    {
        base::glog << "Failed to attach a null component to a node" << base::logwarn;
        return;
    }

    auto idx = component->type;

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

void Node::detachComponent(components::ComponentType type)
{
    auto component = m_components[type];
    if (!component)
    {
        base::glog << "Trying to detach non existing node component" << base::logwarn;
        return;
    }

    component->onDetached();
    m_components[type].reset();
}

shared_ptr<Node> Node::fromFile(const char *jsonDefinition)
{
    auto root = utils::jsonLoadFromFile(jsonDefinition);

    return fromJson(root);
}

shared_ptr<Node> Node::fromJson(const Json::Value &root)
{
    if (root.empty())
    {
        base::glog << "Failed to init scene node from json node" << base::logwarn;
        return nullptr;
    }

    auto externalDataJson = root["external_data"];
    if (!externalDataJson.empty())
        return fromJson(utils::jsonLoadFromFile(externalDataJson.asCString()));

    auto objName = root["name"].asString();
    const auto &componentsJson = root["components"];

    auto result = make_shared<scene::Node>(objName.c_str());
    // FIXME: first attach transform, then other componets!
    for (const auto &component : componentsJson)
        result->attachComponent(components::NodeComponent::fromJson(component));

    const auto &childrenJson = root["children"];
    for (Json::UInt objIdx = 0; objIdx < childrenJson.size(); ++objIdx)
    {
        const auto &childJson = childrenJson[objIdx];
        result->addChild(fromJson(childJson));
    }

    return result;
}

Json::Value Node::toJson(shared_ptr<const Node> node)
{
    Json::Value result(Json::objectValue);
    Json::Value componentsJson(Json::arrayValue);
    Json::Value childrenJson(Json::arrayValue);

    result["name"] = node->getName();

    for (size_t i = 0; i < components::COUNT; i++)
    {
        auto comp = node->getComponent(static_cast<components::ComponentType>(i));
        if (!comp)
            continue;

        componentsJson.append(components::NodeComponent::toJson(comp));
    }

    for (auto it = node->cbegin(); it != node->cend(); it++)
        childrenJson.append(toJson(*it));

    result["components"] = componentsJson;
    result["children"] = childrenJson;

    return result;
}

} }
