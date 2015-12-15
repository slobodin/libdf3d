#pragma once

#include <scene/Entity.h>

namespace df3d {

// TODO_ecs: rename to scenegraph component mb?

class DF3D_DLL TransformComponentProcessor : utils::NonCopyable
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

public:
    TransformComponentProcessor();
    ~TransformComponentProcessor();

    void setPosition(ComponentInstance comp, const glm::vec3 &newPosition);
    void setPosition(ComponentInstance comp, float x, float y, float z);
    void setScale(ComponentInstance comp, const glm::vec3 &newScale);
    void setScale(ComponentInstance comp, float x, float y, float z);
    void setScale(ComponentInstance comp, float uniform);
    void setOrientation(ComponentInstance comp, const glm::quat &newOrientation);
    void setOrientation(ComponentInstance comp, const glm::vec3 &eulerAngles, bool rads = false);

    void translate(ComponentInstance comp, const glm::vec3 &v);
    void translate(ComponentInstance comp, float x, float y, float z);
    void scale(ComponentInstance comp, const glm::vec3 &v);
    void scale(ComponentInstance comp, float x, float y, float z);
    void scale(ComponentInstance comp, float uniform);
    void rotateYaw(ComponentInstance comp, float yaw, bool rads = false);
    void rotatePitch(ComponentInstance comp, float pitch, bool rads = false);
    void rotateRoll(ComponentInstance comp, float roll, bool rads = false);
    void rotateAxis(ComponentInstance comp, float angle, const glm::vec3 &axis, bool rads = false);

    // TODO_ecs: make getWorldPosition & getLocalPosition instead.
    glm::vec3 getPosition(ComponentInstance comp, bool includeParent = false);
    glm::vec3 getScale(ComponentInstance comp, bool includeParent = false);
    glm::quat getOrientation(ComponentInstance comp, bool includeParent = false);
    glm::mat4 getTransformation(ComponentInstance comp);
    glm::vec3 getRotation(ComponentInstance comp, bool rads = false, bool includeParent = false);

    ComponentInstance add(Entity e);
    void remove(Entity e);
    ComponentInstance lookup(Entity e);
};

}
