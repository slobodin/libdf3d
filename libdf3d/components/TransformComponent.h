#pragma once

#include "NodeComponent.h"

namespace df3d {

class DF3D_DLL TransformComponent : public NodeComponent
{
    //! Accumulated node world transform (including parent).
    glm::mat4 m_transformation;
    //! Node local position.
    glm::vec3 m_position;
    //! Node local orientation.
    glm::quat m_orientation;
    //! Node local scale.
    glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

    enum
    {
        POSITION_DIRTY = 1 << 0,
        ORIENTATION_DIRTY = 1 << 1,
        SCALE_DIRTY = 1 << 2
    };

    int m_dirtyBits = POSITION_DIRTY | ORIENTATION_DIRTY | SCALE_DIRTY;

    void updateTransformation();
    void markDirty(int dirtyBits);

public:
    TransformComponent();

    void setPosition(const glm::vec3 &newPosition);
    void setPosition(float x, float y, float z);
    void setScale(const glm::vec3 &newScale);
    void setScale(float x, float y, float z);
    void setScale(float uniform);
    void setOrientation(const glm::quat &newOrientation);
    void setOrientation(const glm::vec3 &eulerAngles, bool rads = false);

    void translate(const glm::vec3 &v);
    void translate(float x, float y, float z);
    void scale(const glm::vec3 &v);
    void scale(float x, float y, float z);
    void scale(float uniform);
    void rotateYaw(float yaw, bool rads = false);
    void rotatePitch(float pitch, bool rads = false);
    void rotateRoll(float roll, bool rads = false);
    void rotateAxis(float angle, const glm::vec3 &axis, bool rads = false);

    glm::vec3 getPosition(bool includeParent = false);
    glm::vec3 getScale(bool includeParent = false);
    glm::quat getOrientation(bool includeParent = false);
    glm::mat4 getTransformation();
    glm::vec3 getRotation(bool rads = false, bool includeParent = false);

    shared_ptr<NodeComponent> clone() const override;
};

}
