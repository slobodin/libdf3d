#include "df3d_pch.h"
#include "TransformComponent.h"

#include <scene/Node.h>
#include <utils/JsonHelpers.h>
#include "serializers/TransformComponentSerializer.h"

namespace df3d { namespace components {

void TransformComponent::updateTransformation()
{
    if (!m_dirty)
        return;

    // Scale -> Rotation -> Translation
    auto tr = glm::translate(m_position) * glm::toMat4(m_orientation) * glm::scale(m_scale);

    // tr = parent * me
    auto parent = m_holder->getParent();
    if (parent && parent->transform())
        m_transformation = parent->transform()->getTransformation() * tr;
    else
        m_transformation = tr;

    markDirty(false);

    sendEvent(ComponentEvent::TRANFORM_CHANGED);
}

void TransformComponent::markDirty(bool dirty)
{
    m_dirty = dirty;

    if (!m_holder)
        return;

    if (dirty)
    {
        for (auto it = m_holder->begin(); it != m_holder->end(); it++)
            it->second->transform()->markDirty(dirty);
    }
}

TransformComponent::TransformComponent()
    : NodeComponent(TRANSFORM)
{

}

void TransformComponent::setPosition(const glm::vec3 &newPosition)
{
    m_position = newPosition;
    markDirty(true);
}

void TransformComponent::setPosition(float x, float y, float z)
{
    setPosition(glm::vec3(x, y, z));
}

void TransformComponent::setScale(const glm::vec3 &newScale)
{
    m_scale = newScale;
    markDirty(true);
}

void TransformComponent::setScale(float x, float y, float z)
{
    setScale(glm::vec3(x, y, z));
}

void TransformComponent::setScale(float uniform)
{
    setScale(uniform, uniform, uniform);
}

void TransformComponent::setOrientation(const glm::quat &newOrientation)
{
    m_orientation = newOrientation;
    markDirty(true);
}

void TransformComponent::setOrientation(const glm::vec3 &eulerAngles, bool rads)
{
    glm::vec3 v;
    if (!rads)
        v = glm::radians(eulerAngles);
    else
        v = eulerAngles;

    setOrientation(glm::quat(v));
}

void TransformComponent::setTransformation(const glm::mat4 &tr)
{
    // FIXME:
    //auto parent = m_holder->getParent();
    //if (parent)
    //    m_transformation = parent->transform()->getTransformation() * tr;
    //else
    //    m_transformation = tr;

    m_transformation = tr;
    markDirty(false);

    sendEvent(ComponentEvent::TRANFORM_CHANGED);
}

void TransformComponent::translate(const glm::vec3 &v)
{
    m_position += v;
    markDirty(true);
}

void TransformComponent::translate(float x, float y, float z)
{
    translate(glm::vec3(x, y, z));
}

void TransformComponent::scale(const glm::vec3 &v)
{
    scale(v.x, v.y, v.z);
}

void TransformComponent::scale(float x, float y, float z)
{
    m_scale.x *= x;
    m_scale.y *= y;
    m_scale.z *= z;
    markDirty(true);
}

void TransformComponent::scale(float uniform)
{
    scale(uniform, uniform, uniform);
}

void TransformComponent::rotateYaw(float yaw, bool rads)
{
    rotateAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f), rads);
}

void TransformComponent::rotatePitch(float pitch, bool rads)
{
    rotateAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f), rads);
}

void TransformComponent::rotateRoll(float roll, bool rads)
{
    rotateAxis(roll, glm::vec3(0.0f, 0.0f, 1.0f), rads);
}

void TransformComponent::rotateAxis(float angle, const glm::vec3 &axis, bool rads)
{
    if (!rads)
        angle = glm::radians(angle);

    auto q = glm::angleAxis(angle, axis);
    q = glm::normalize(q);

    m_orientation = q * m_orientation;
    markDirty(true);
}

glm::vec3 TransformComponent::getPosition()
{
    updateTransformation();

    return m_position;
}

glm::vec3 TransformComponent::getScale()
{
    updateTransformation();

    return m_scale;
}

glm::quat TransformComponent::getOrientation()
{
    updateTransformation();

    return m_orientation;
}

glm::mat4 TransformComponent::getTransformation()
{
    updateTransformation();

    return m_transformation;
}

glm::vec3 TransformComponent::getRotation(bool rads)
{
    updateTransformation();

    if (rads)
        return glm::eulerAngles(m_orientation);
    else
        return glm::degrees(glm::eulerAngles(m_orientation));
}

shared_ptr<NodeComponent> TransformComponent::clone() const
{
    auto result = make_shared<TransformComponent>();

    result->m_transformation = m_transformation;
    result->m_position = m_position;
    result->m_orientation = m_orientation;
    result->m_scale = m_scale;
    result->m_dirty = m_dirty;

    return result;
}

} }