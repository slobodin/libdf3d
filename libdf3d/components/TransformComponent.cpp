#include "TransformComponent.h"

#include <scene/Node.h>
#include <utils/JsonUtils.h>

namespace df3d {

void TransformComponent::updateTransformation()
{
    if (!m_dirtyBits)
        return;

    // Scale -> Rotation -> Translation
    auto tr = glm::translate(m_position) * glm::toMat4(m_orientation) * glm::scale(m_scale);

    // tr = parent * me
    auto parent = m_holder->getParent();
    if (parent && parent->transform())
        m_transformation = parent->transform()->getTransformation() * tr;
    else
        m_transformation = tr;

    bool positionChanged = m_dirtyBits & POSITION_DIRTY;
    bool orientationChanged = m_dirtyBits & ORIENTATION_DIRTY;
    bool scaleChanged = m_dirtyBits & SCALE_DIRTY;

    markDirty(0);

    if (positionChanged)
        sendEvent(ComponentEvent::POSITION_CHANGED);
    if (orientationChanged)
        sendEvent(ComponentEvent::ORIENTATION_CHANGED);
    if (scaleChanged)
        sendEvent(ComponentEvent::SCALE_CHANGED);
}

void TransformComponent::markDirty(int dirtyBits)
{
    m_dirtyBits = dirtyBits;

    if (!m_holder)
        return;

    if (m_dirtyBits)
    {
        for (auto it : *m_holder)
            it->transform()->markDirty(m_dirtyBits);
    }
}

TransformComponent::TransformComponent()
    : NodeComponent(ComponentType::TRANSFORM)
{

}

void TransformComponent::setPosition(const glm::vec3 &newPosition)
{
    m_position = newPosition;
    markDirty(m_dirtyBits | POSITION_DIRTY);
}

void TransformComponent::setPosition(float x, float y, float z)
{
    setPosition(glm::vec3(x, y, z));
}

void TransformComponent::setScale(const glm::vec3 &newScale)
{
    m_scale = newScale;
    markDirty(m_dirtyBits | SCALE_DIRTY);
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
    markDirty(m_dirtyBits | ORIENTATION_DIRTY);
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

void TransformComponent::translate(const glm::vec3 &v)
{
    m_position += v;
    markDirty(m_dirtyBits | POSITION_DIRTY);
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
    markDirty(m_dirtyBits | SCALE_DIRTY);
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
    markDirty(m_dirtyBits | ORIENTATION_DIRTY);
}

glm::vec3 TransformComponent::getPosition(bool includeParent)
{
    updateTransformation();

    if (!includeParent)
        return m_position;
    else
        return glm::vec3(m_transformation[3]);
}

glm::vec3 TransformComponent::getScale(bool includeParent)
{
    updateTransformation();

    return m_scale;
}

glm::quat TransformComponent::getOrientation(bool includeParent)
{
    updateTransformation();

    return m_orientation;
}

glm::mat4 TransformComponent::getTransformation()
{
    updateTransformation();

    return m_transformation;
}

glm::vec3 TransformComponent::getRotation(bool rads, bool includeParent)
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
    result->m_dirtyBits = m_dirtyBits;

    return result;
}

}
