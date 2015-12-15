#include "df3d_pch.h"
#include "TransformComponentProcessor.h"

#include <scene/impl/ComponentDataHolder.h>

namespace df3d {

struct TransformComponentProcessor::Impl
{
    // TODO_ecs: play with layout.
    // TODO_ecs: return dirty flag back?
    struct Data
    {
        //! Accumulated node world transform (including parent).
        glm::mat4 transformation;
        //! Node local position.
        glm::vec3 position;
        //! Node local orientation.
        glm::quat orientation;
        //! Node local scale.
        glm::vec3 scaling = glm::vec3(1.0f, 1.0f, 1.0f);

        ComponentInstance parent;
        std::vector<ComponentInstance> children;

        std::string nodeName;
    };

    ComponentDataHolder<Data> data;

    void updateWorldTransformation(Data &component)
    {
        // Scale -> Rotation -> Translation
        auto tr = glm::translate(component.position) * glm::toMat4(component.orientation) * glm::scale(component.scaling);

        if (component.parent.valid())
            component.transformation = data.getData(component.parent).transformation * tr; // tr = parent * me
        else
            component.transformation = tr;

        for (auto child : component.children)
            updateWorldTransformation(data.getData(child));
    }
};

TransformComponentProcessor::TransformComponentProcessor()
{

}

TransformComponentProcessor::~TransformComponentProcessor()
{

}

void TransformComponentProcessor::setPosition(ComponentInstance comp, const glm::vec3 &newPosition)
{
    auto &compData = m_pimpl->data.getData(comp);

    compData.position = newPosition;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::setPosition(ComponentInstance comp, float x, float y, float z)
{
    setPosition(comp, glm::vec3(x, y, z));
}

void TransformComponentProcessor::setScale(ComponentInstance comp, const glm::vec3 &newScale)
{
    auto &compData = m_pimpl->data.getData(comp);

    compData.scaling = newScale;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::setScale(ComponentInstance comp, float x, float y, float z)
{
    setScale(comp, glm::vec3(x, y, z));
}

void TransformComponentProcessor::setScale(ComponentInstance comp, float uniform)
{
    setScale(comp, uniform, uniform, uniform);
}

void TransformComponentProcessor::setOrientation(ComponentInstance comp, const glm::quat &newOrientation)
{
    auto &compData = m_pimpl->data.getData(comp);

    compData.orientation = newOrientation;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::setOrientation(ComponentInstance comp, const glm::vec3 &eulerAngles, bool rads)
{
    glm::vec3 v;
    if (!rads)
        v = glm::radians(eulerAngles);
    else
        v = eulerAngles;

    setOrientation(comp, glm::quat(v));
}

void TransformComponentProcessor::translate(ComponentInstance comp, const glm::vec3 &v)
{
    auto &compData = m_pimpl->data.getData(comp);

    compData.position += v;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::translate(ComponentInstance comp, float x, float y, float z)
{
    translate(comp, glm::vec3(x, y, z));
}

void TransformComponentProcessor::scale(ComponentInstance comp, const glm::vec3 &v)
{
    scale(comp, v.x, v.y, v.z);
}

void TransformComponentProcessor::scale(ComponentInstance comp, float x, float y, float z)
{
    auto &compData = m_pimpl->data.getData(comp);

    compData.scaling *= glm::vec3(x, y, z);

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::scale(ComponentInstance comp, float uniform)
{
    scale(comp, uniform, uniform, uniform);
}

void TransformComponentProcessor::rotateYaw(ComponentInstance comp, float yaw, bool rads)
{
    rotateAxis(comp, yaw, glm::vec3(0.0f, 1.0f, 0.0f), rads);
}

void TransformComponentProcessor::rotatePitch(ComponentInstance comp, float pitch, bool rads)
{
    rotateAxis(comp, pitch, glm::vec3(1.0f, 0.0f, 0.0f), rads);
}

void TransformComponentProcessor::rotateRoll(ComponentInstance comp, float roll, bool rads)
{
    rotateAxis(comp, roll, glm::vec3(0.0f, 0.0f, 1.0f), rads);
}

void TransformComponentProcessor::rotateAxis(ComponentInstance comp, float angle, const glm::vec3 &axis, bool rads)
{
    if (!rads)
        angle = glm::radians(angle);

    auto q = glm::angleAxis(angle, axis);
    q = glm::normalize(q);

    auto &compData = m_pimpl->data.getData(comp);
    compData.orientation = q * compData.orientation;

    m_pimpl->updateWorldTransformation(compData);
}

glm::vec3 TransformComponentProcessor::getPosition(ComponentInstance comp, bool includeParent)
{
    const auto &compData = m_pimpl->data.getData(comp);

    if (!includeParent)
        return compData.position;
    else
        return glm::vec3(compData.transformation[3]);
}

glm::vec3 TransformComponentProcessor::getScale(ComponentInstance comp)
{
    return m_pimpl->data.getData(comp).scaling;
}

glm::quat TransformComponentProcessor::getOrientation(ComponentInstance comp)
{
    return m_pimpl->data.getData(comp).orientation;
}

glm::mat4 TransformComponentProcessor::getTransformation(ComponentInstance comp)
{
    return m_pimpl->data.getData(comp).transformation;
}

glm::vec3 TransformComponentProcessor::getRotation(ComponentInstance comp, bool rads)
{
    const auto &compData = m_pimpl->data.getData(comp);

    if (rads)
        return glm::eulerAngles(compData.orientation);
    else
        return glm::degrees(glm::eulerAngles(compData.orientation));
}

void TransformComponentProcessor::addChild(ComponentInstance parent, ComponentInstance child)
{
    if (getParent(child).valid())
    {
        glog << "Can not add child entity. An entity already has a parent" << logwarn;
        return;
    }

    auto &parentCompData = m_pimpl->data.getData(parent);
    auto &childCompData = m_pimpl->data.getData(child);

    parentCompData.children.push_back(child);
    childCompData.parent = parent;
}

void TransformComponentProcessor::removeChild(ComponentInstance parent, ComponentInstance child)
{
    // TODO_ecs:
    assert(false);
}

void TransformComponentProcessor::removeAllChildren(ComponentInstance comp)
{
    // TODO_ecs:
    assert(false);
}

ComponentInstance TransformComponentProcessor::getParent(ComponentInstance comp)
{
    return m_pimpl->data.getData(comp).parent;
}

ComponentInstance TransformComponentProcessor::add(Entity e, const std::string &name)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has an scene graph component" << logwarn;
        return ComponentInstance();
    }

    Impl::Data data;
    data.nodeName = name;
    if (data.nodeName.empty())
    {
        static uint32_t nodesCount;
        data.nodeName = std::string("unnamed_node_") + utils::to_string(nodesCount++);
    }

    return m_pimpl->data.add(e, data);
}

void TransformComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove scene graph component from an entity. Component is not attached" << logwarn;
        return;
    }

    auto compInst = m_pimpl->data.lookup(e);

    // TODO_ecs: parent & children
    assert(false);

    m_pimpl->data.remove(e);
}

ComponentInstance TransformComponentProcessor::lookup(Entity e)
{
    return m_pimpl->data.lookup(e);
}

}
