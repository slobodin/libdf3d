#include "df3d_pch.h"
#include "TransformComponentProcessor.h"

#include <game/ComponentDataHolder.h>

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

        Entity parent;
        std::vector<Entity> children;
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

void TransformComponentProcessor::remove(Entity e)
{
    // FIXME: only this system can remove transform component.

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

void TransformComponentProcessor::update(float systemDelta, float gameDelta)
{

}

void TransformComponentProcessor::cleanStep(World &w)
{

}

TransformComponentProcessor::TransformComponentProcessor()
    : m_pimpl(new Impl())
{

}

TransformComponentProcessor::~TransformComponentProcessor()
{

}

void TransformComponentProcessor::setPosition(Entity e, const glm::vec3 &newPosition)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.position = newPosition;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::setPosition(Entity e, float x, float y, float z)
{
    setPosition(e, glm::vec3(x, y, z));
}

void TransformComponentProcessor::setScale(Entity e, const glm::vec3 &newScale)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.scaling = newScale;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::setScale(Entity e, float x, float y, float z)
{
    setScale(e, glm::vec3(x, y, z));
}

void TransformComponentProcessor::setScale(Entity e, float uniform)
{
    setScale(e, uniform, uniform, uniform);
}

void TransformComponentProcessor::setOrientation(Entity e, const glm::quat &newOrientation)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.orientation = newOrientation;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::setOrientation(Entity e, const glm::vec3 &eulerAngles, bool rads)
{
    glm::vec3 v = rads ? eulerAngles : glm::radians(eulerAngles);

    setOrientation(e, glm::quat(v));
}

void TransformComponentProcessor::translate(Entity e, const glm::vec3 &v)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.position += v;

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::translate(Entity e, float x, float y, float z)
{
    translate(e, glm::vec3(x, y, z));
}

void TransformComponentProcessor::scale(Entity e, const glm::vec3 &v)
{
    scale(e, v.x, v.y, v.z);
}

void TransformComponentProcessor::scale(Entity e, float x, float y, float z)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.scaling *= glm::vec3(x, y, z);

    m_pimpl->updateWorldTransformation(compData);
}

void TransformComponentProcessor::scale(Entity e, float uniform)
{
    scale(e, uniform, uniform, uniform);
}

void TransformComponentProcessor::rotateYaw(Entity e, float yaw, bool rads)
{
    rotateAxis(e, yaw, glm::vec3(0.0f, 1.0f, 0.0f), rads);
}

void TransformComponentProcessor::rotatePitch(Entity e, float pitch, bool rads)
{
    rotateAxis(e, pitch, glm::vec3(1.0f, 0.0f, 0.0f), rads);
}

void TransformComponentProcessor::rotateRoll(Entity e, float roll, bool rads)
{
    rotateAxis(e, roll, glm::vec3(0.0f, 0.0f, 1.0f), rads);
}

void TransformComponentProcessor::rotateAxis(Entity e, float angle, const glm::vec3 &axis, bool rads)
{
    if (!rads)
        angle = glm::radians(angle);

    auto q = glm::angleAxis(angle, axis);
    q = glm::normalize(q);

    auto &compData = m_pimpl->data.getData(e);
    compData.orientation = q * compData.orientation;

    m_pimpl->updateWorldTransformation(compData);
}

glm::vec3 TransformComponentProcessor::getPosition(Entity e, bool includeParent)
{
    const auto &compData = m_pimpl->data.getData(e);

    if (!includeParent)
        return compData.position;
    else
        return glm::vec3(compData.transformation[3]);
}

glm::vec3 TransformComponentProcessor::getScale(Entity e)
{
    return m_pimpl->data.getData(e).scaling;
}

glm::quat TransformComponentProcessor::getOrientation(Entity e)
{
    return m_pimpl->data.getData(e).orientation;
}

glm::mat4 TransformComponentProcessor::getTransformation(Entity e)
{
    return m_pimpl->data.getData(e).transformation;
}

glm::vec3 TransformComponentProcessor::getRotation(Entity e, bool rads)
{
    const auto &compData = m_pimpl->data.getData(e);

    if (rads)
        return glm::eulerAngles(compData.orientation);
    else
        return glm::degrees(glm::eulerAngles(compData.orientation));
}

void TransformComponentProcessor::addChild(Entity parent, Entity child)
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

void TransformComponentProcessor::removeChild(Entity parent, Entity child)
{
    // TODO_ecs:
    assert(false);
}

void TransformComponentProcessor::removeAllChildren(Entity e)
{
    // TODO_ecs:
    assert(false);
}

Entity TransformComponentProcessor::getParent(Entity e)
{
    return m_pimpl->data.getData(e).parent;
}

void TransformComponentProcessor::add(Entity e)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has an scene graph component" << logwarn;
        return;
    }

    m_pimpl->data.add(e, Impl::Data());
}

}
