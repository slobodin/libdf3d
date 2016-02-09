#pragma once

#include <libdf3d/game/Entity.h>
#include <libdf3d/game/EntityComponentProcessor.h>

namespace df3d {

class DF3D_DLL SceneGraphComponentProcessor : public EntityComponentProcessor
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update() override;
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    SceneGraphComponentProcessor();
    ~SceneGraphComponentProcessor();

    void setPosition(Entity e, const glm::vec3 &newPosition);
    void setScale(Entity e, const glm::vec3 &newScale);
    void setScale(Entity e, float uniform);
    void setOrientation(Entity e, const glm::quat &newOrientation);
    void setOrientation(Entity e, const glm::vec3 &eulerAngles, bool rads = false);
    // NOTE: this is used by physics processor. FIXME
    void setTransform(Entity e, const glm::vec3 &position, const glm::quat &orient, const glm::mat4 &transf);

    void translate(Entity e, const glm::vec3 &v);
    void scale(Entity e, const glm::vec3 &v);
    void scale(Entity e, float uniform);
    void rotateYaw(Entity e, float yaw, bool rads = false);
    void rotatePitch(Entity e, float pitch, bool rads = false);
    void rotateRoll(Entity e, float roll, bool rads = false);
    void rotateAxis(Entity e, float angle, const glm::vec3 &axis, bool rads = false);

    void setName(Entity e, const std::string &name);
    const std::string& getName(Entity e) const;
    Entity getByName(const std::string &name) const;
    Entity getByName(Entity parent, const std::string &name) const;

    // TODO_ecs: make getWorldPosition & getLocalPosition instead.
    glm::vec3 getPosition(Entity e, bool includeParent = false);
    const glm::vec3& getScale(Entity e);
    const glm::quat& getOrientation(Entity e);
    const glm::mat4& getTransformation(Entity e);
    void getTransformation(Entity e, glm::mat4 &outTr, glm::vec3 &outPos, glm::quat &outRot, glm::vec3 &outScale);
    glm::vec3 getRotation(Entity e, bool rads = false);
    glm::vec3 getWorldDirection(Entity e);

    void attachChild(Entity parent, Entity child);
    void attachChildren(Entity parent, const std::vector<Entity> &children);
    void detachChild(Entity parent, Entity child);
    void detachAllChildren(Entity e);
    Entity getParent(Entity e);
    const std::vector<Entity>& getChildren(Entity e) const;

    void add(Entity e);
};

}
