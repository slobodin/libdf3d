#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>

class btTransform;

namespace df3d {

struct Transform
{
    glm::mat4 combined;
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 scaling = glm::vec3(1.0f, 1.0f, 1.0f);
};

class SceneGraphComponentProcessor : public EntityComponentProcessor
{
    struct Data
    {
        Transform wTransform;   // world transform
        Transform lTransform;   // local transform
        std::string name;
        Entity parent;
        Entity holder;
        std::vector<Entity> children;
        bool localTransformDirty = true;
    };

    ComponentDataHolder<Data> m_data;

    void updateWorldTransformation(Data &component);
    void updateLocalTransform(Data &component);
    void updateChildren(Data &component);

    void update() override { }

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

    glm::vec3 getWorldPosition(Entity e) const;
    glm::quat getWorldOrientation(Entity e) const;
    glm::vec3 getWorldRotation(Entity e) const;

    glm::vec3 getLocalPosition(Entity e) const;
    glm::vec3 getLocalScale(Entity e) const;
    glm::quat getLocalOrientation(Entity e) const;
    glm::vec3 getLocalRotation(Entity e) const;

    glm::mat4 getWorldTransformMatrix(Entity e) const;
    Transform getWorldTransform(Entity e) const;

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
    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
