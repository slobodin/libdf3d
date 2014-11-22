#pragma once

#include "NodeComponent.h"

namespace df3d { namespace components {

class DF3D_DLL TransformComponent : public NodeComponent
{
    glm::mat4 m_transformation;
    glm::vec3 m_position;
    glm::quat m_orientation;
    glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    bool m_dirty = true;

    void updateTransformation();
    void markDirty(bool dirty);

public:
    TransformComponent();
    TransformComponent(const Json::Value &root);

    void setPosition(const glm::vec3 &newPosition);
    void setPosition(float x, float y, float z);
    void setScale(const glm::vec3 &newScale);
    void setScale(float x, float y, float z);
    void setOrientation(const glm::quat &newOrientation);
    void setOrientation(const glm::vec3 &eulerAngles, bool rads = false);
    void setTransformation(const glm::mat4 &tr);

    void translate(const glm::vec3 &v);
    void translate(float x, float y, float z);
    void scale(const glm::vec3 &v);
    void scale(float x, float y, float z);
    void rotateYaw(float yaw, bool rads = false);
    void rotatePitch(float pitch, bool rads = false);
    void rotateRoll(float roll, bool rads = false);
    void rotateAxis(float angle, const glm::vec3 &axis, bool rads = false);

    glm::vec3 getPosition();
    glm::vec3 getScale();
    glm::quat getOrientation();
    glm::mat4 getTransformation();
    glm::vec3 getRotation(bool rads = false);

    shared_ptr<NodeComponent> clone() const override;
};

} }