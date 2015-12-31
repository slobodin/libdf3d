#pragma once

#include <game/Entity.h>
#include <game/EntityComponentProcessor.h>

namespace df3d {

// TODO_ecs: rename to scenegraph component mb?

class DF3D_DLL TransformComponentProcessor : public EntityComponentProcessor
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update() override;
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    TransformComponentProcessor();
    ~TransformComponentProcessor();

    void setPosition(Entity e, const glm::vec3 &newPosition);
    void setPosition(Entity e, float x, float y, float z);
    void setScale(Entity e, const glm::vec3 &newScale);
    void setScale(Entity e, float x, float y, float z);
    void setScale(Entity e, float uniform);
    void setOrientation(Entity e, const glm::quat &newOrientation);
    void setOrientation(Entity e, const glm::vec3 &eulerAngles, bool rads = false);
    // NOTE: this is used by physics processor. FIXME
    void setTransform(Entity e, const glm::vec3 &position, const glm::quat &orient, const glm::mat4 &transf);

    void translate(Entity e, const glm::vec3 &v);
    void translate(Entity e, float x, float y, float z);
    void scale(Entity e, const glm::vec3 &v);
    void scale(Entity e, float x, float y, float z);
    void scale(Entity e, float uniform);
    void rotateYaw(Entity e, float yaw, bool rads = false);
    void rotatePitch(Entity e, float pitch, bool rads = false);
    void rotateRoll(Entity e, float roll, bool rads = false);
    void rotateAxis(Entity e, float angle, const glm::vec3 &axis, bool rads = false);

    // TODO_ecs: make getWorldPosition & getLocalPosition instead.
    glm::vec3 getPosition(Entity e, bool includeParent = false);
    const glm::vec3& getScale(Entity e);
    const glm::quat& getOrientation(Entity e);
    const glm::mat4& getTransformation(Entity e);
    glm::vec3 getRotation(Entity e, bool rads = false);

    void attachChild(Entity parent, Entity child);
    void detachChild(Entity parent, Entity child);
    void detachAllChildren(Entity e);
    Entity getParent(Entity e);

    void add(Entity e);
};

}
