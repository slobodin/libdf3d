#pragma once

#include <libdf3d/game/Entity.h>
#include <libdf3d/game/EntityComponentProcessor.h>

class btTransform;

namespace df3d {

struct Transform
{
    glm::mat4 combined;
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 scaling = glm::vec3(1.0f, 1.0f, 1.0f);
};

class DF3D_DLL SceneGraphComponentProcessor : public EntityComponentProcessor
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update() override;
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    SceneGraphComponentProcessor();
    ~SceneGraphComponentProcessor();

    //! Sets new local position for an entity.
    void setPosition(Entity e, const glm::vec3 &newPosition);
    //! Sets new local scale for an entity.
    void setScale(Entity e, const glm::vec3 &newScale);
    //! Sets uniform local scale for an entity.
    void setScale(Entity e, float uniform);
    //! Sets orientation.
    void setOrientation(Entity e, const glm::quat &newOrientation);
    //! Sets orientation using Euler angles (degrees).
    void setOrientation(Entity e, const glm::vec3 &eulerAngles);
    // NOTE: used by physics component processor.
    void setWorldTransform(Entity e, const btTransform &worldTrans);

    void translate(Entity e, const glm::vec3 &v);
    void scale(Entity e, const glm::vec3 &v);
    void scale(Entity e, float uniform);
    void rotateYaw(Entity e, float yaw);
    void rotatePitch(Entity e, float pitch);
    void rotateRoll(Entity e, float roll);
    void rotateAxis(Entity e, float angle, const glm::vec3 &axis);

    void setName(Entity e, const std::string &name);
    const std::string& getName(Entity e) const;
    Entity getByName(const std::string &name) const;
    Entity getByName(Entity parent, const std::string &name) const;

    const glm::vec3& getWorldPosition(Entity e) const;
    const glm::quat& getWorldOrientation(Entity e) const;
    glm::vec3 getWorldRotation(Entity e) const;

    glm::vec3 getLocalPosition(Entity e) const;
    const glm::vec3& getLocalScale(Entity e) const;
    const glm::quat& getLocalOrientation(Entity e) const;
    glm::vec3 getLocalRotation(Entity e) const;

    const glm::mat4& getWorldTransformMatrix(Entity e) const;
    const Transform& getWorldTransform(Entity e) const;

    glm::vec3 getWorldDirection(Entity e) const;
    glm::vec3 getWorldUp(Entity e) const;
    glm::vec3 getWorldRight(Entity e) const;

    void attachChild(Entity parent, Entity child);
    void attachChildren(Entity parent, const std::vector<Entity> &children);
    void detachChild(Entity parent, Entity child);
    void detachAllChildren(Entity e);
    Entity getParent(Entity e);
    const std::vector<Entity>& getChildren(Entity e) const;

    void add(Entity e);
};

}
