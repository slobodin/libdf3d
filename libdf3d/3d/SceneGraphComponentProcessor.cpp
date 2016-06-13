#include "SceneGraphComponentProcessor.h"

#include <LinearMath/btTransform.h>
#include <libdf3d/game/ComponentDataHolder.h>
#include <libdf3d/physics/PhysicsHelpers.h>
#include <libdf3d/utils/MathUtils.h>

namespace df3d {

struct SceneGraphComponentProcessor::Impl
{
    // TODO_ecs: return dirty flag back?
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

    ComponentDataHolder<Data> data;

    void updateWorldTransformation(Data &component)
    {
        updateLocalTransform(component);

        if (component.parent.valid())
        {
            auto &myWTransform = component.wTransform;
            const auto &myLTransform = component.lTransform;
            const auto &parentWTransform = data.getData(component.parent).wTransform;

            myWTransform.combined = parentWTransform.combined * myLTransform.combined; // tr = parent * me

            component.wTransform.position = glm::vec3(component.wTransform.combined[3]);
            component.wTransform.orientation = parentWTransform.orientation * myLTransform.orientation;
            component.wTransform.scaling.x = parentWTransform.scaling.x * myLTransform.scaling.x;
            component.wTransform.scaling.y = parentWTransform.scaling.y * myLTransform.scaling.y;
            component.wTransform.scaling.y = parentWTransform.scaling.y * myLTransform.scaling.y;
        }
        else
        {
            component.wTransform = component.lTransform;
        }

        updateChildren(component);
    }

    void updateLocalTransform(Data &component)
    {
        if (component.localTransformDirty)
        {
            // Scale -> Rotation -> Translation
            auto &lTransform = component.lTransform;
            lTransform.combined = glm::translate(lTransform.position) * glm::toMat4(lTransform.orientation) * glm::scale(lTransform.scaling);
            component.localTransformDirty = false;
        }
    }

    void updateChildren(Data &component)
    {
        for (auto child : component.children)
            updateWorldTransformation(data.getData(child));
    }
};

void SceneGraphComponentProcessor::update()
{

}

void SceneGraphComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    if (!deleted.empty())
    {
        for (auto it : deleted)
        {
            auto &compData = m_pimpl->data.getData(it);
            for (auto child : compData.children)
                m_pimpl->data.getData(child).parent = Entity();

            if (compData.parent.valid())
                detachChild(compData.parent, it);
        }

        m_pimpl->data.cleanStep(deleted);
    }
}

SceneGraphComponentProcessor::SceneGraphComponentProcessor()
    : m_pimpl(new Impl())
{

}

SceneGraphComponentProcessor::~SceneGraphComponentProcessor()
{

}

void SceneGraphComponentProcessor::setPosition(Entity e, const glm::vec3 &newPosition)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.lTransform.position = newPosition;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setScale(Entity e, const glm::vec3 &newScale)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.lTransform.scaling = newScale;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setScale(Entity e, float uniform)
{
    setScale(e, { uniform, uniform, uniform });
}

void SceneGraphComponentProcessor::setOrientation(Entity e, const glm::quat &newOrientation)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.lTransform.orientation = newOrientation;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setOrientation(Entity e, const glm::vec3 &eulerAngles)
{
    setOrientation(e, glm::quat(glm::radians(eulerAngles)));
}

void SceneGraphComponentProcessor::setWorldTransform(Entity e, const btTransform &worldTrans)
{
    auto &compData = m_pimpl->data.getData(e);

    DF3D_ASSERT_MESS(!compData.parent.valid(), "physics not supported for children entities");

    const auto btRot = worldTrans.getRotation();
    glm::mat4 ATTRIBUTE_ALIGNED16(df3dWorldTransf);
    worldTrans.getOpenGLMatrix(glm::value_ptr(df3dWorldTransf));

    compData.wTransform.combined = df3dWorldTransf * glm::scale(compData.lTransform.scaling);
    compData.wTransform.orientation = glm::quat(btRot.w(), btRot.x(), btRot.y(), btRot.z());
    compData.wTransform.position = btToGlm(worldTrans.getOrigin());
    compData.wTransform.scaling = compData.lTransform.scaling;
    compData.lTransform = compData.wTransform;
    compData.localTransformDirty = false;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::translate(Entity e, const glm::vec3 &v)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.lTransform.position += v;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::scale(Entity e, const glm::vec3 &v)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.lTransform.scaling *= v;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::scale(Entity e, float uniform)
{
    scale(e, { uniform, uniform, uniform });
}

void SceneGraphComponentProcessor::rotateYaw(Entity e, float yaw)
{
    rotateAxis(e, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
}

void SceneGraphComponentProcessor::rotatePitch(Entity e, float pitch)
{
    rotateAxis(e, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
}

void SceneGraphComponentProcessor::rotateRoll(Entity e, float roll)
{
    rotateAxis(e, roll, glm::vec3(0.0f, 0.0f, 1.0f));
}

void SceneGraphComponentProcessor::rotateAxis(Entity e, float angle, const glm::vec3 &axis)
{
    auto q = glm::angleAxis(glm::radians(angle), axis);
    q = glm::normalize(q);

    auto &compData = m_pimpl->data.getData(e);
    compData.lTransform.orientation = q * compData.lTransform.orientation;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setName(Entity e, const std::string &name)
{
    m_pimpl->data.getData(e).name = name;
}

const std::string& SceneGraphComponentProcessor::getName(Entity e) const
{
    return m_pimpl->data.getData(e).name;
}

Entity SceneGraphComponentProcessor::getByName(const std::string &name) const
{
    if (name.empty())
        return {};

    for (const auto &compData : m_pimpl->data.rawData())
    {
        if (!compData.parent.valid() && compData.name == name)
            return compData.holder;
    }

    return {};
}

Entity SceneGraphComponentProcessor::getByName(Entity parent, const std::string &name) const
{
    if (name.empty())
        return {};

    DF3D_ASSERT(parent.valid());

    for (const auto &compData : m_pimpl->data.rawData())
    {
        if (compData.parent == parent && compData.name == name)
            return compData.holder;
    }

    return {};
}

const glm::vec3& SceneGraphComponentProcessor::getWorldPosition(Entity e) const
{
    return m_pimpl->data.getData(e).wTransform.position;
}

const glm::quat& SceneGraphComponentProcessor::getWorldOrientation(Entity e) const
{
    return m_pimpl->data.getData(e).wTransform.orientation;
}

glm::vec3 SceneGraphComponentProcessor::getWorldRotation(Entity e) const
{
    const auto &compData = m_pimpl->data.getData(e);

    return glm::degrees(glm::eulerAngles(compData.wTransform.orientation));
}

glm::vec3 SceneGraphComponentProcessor::getLocalPosition(Entity e) const
{
    return m_pimpl->data.getData(e).lTransform.position;
}

const glm::vec3& SceneGraphComponentProcessor::getLocalScale(Entity e) const
{
    return m_pimpl->data.getData(e).lTransform.scaling;
}

const glm::quat& SceneGraphComponentProcessor::getLocalOrientation(Entity e) const
{
    return m_pimpl->data.getData(e).lTransform.orientation;
}

glm::vec3 SceneGraphComponentProcessor::getLocalRotation(Entity e) const
{
    const auto &compData = m_pimpl->data.getData(e);

    return glm::degrees(glm::eulerAngles(compData.lTransform.orientation));
}

const glm::mat4& SceneGraphComponentProcessor::getWorldTransformMatrix(Entity e) const
{
    return m_pimpl->data.getData(e).wTransform.combined;
}

const Transform& SceneGraphComponentProcessor::getWorldTransform(Entity e) const
{
    return m_pimpl->data.getData(e).wTransform;
}

glm::vec3 SceneGraphComponentProcessor::getWorldDirection(Entity e) const
{
    return glm::normalize(glm::vec3(getWorldTransformMatrix(e) * -utils::math::ZAxis));
}

glm::vec3 SceneGraphComponentProcessor::getWorldUp(Entity e) const
{
    return glm::normalize(glm::vec3(getWorldTransformMatrix(e) * utils::math::YAxis));
}

glm::vec3 SceneGraphComponentProcessor::getWorldRight(Entity e) const
{
    return glm::normalize(glm::vec3(getWorldTransformMatrix(e) * utils::math::XAxis));
}

void SceneGraphComponentProcessor::attachChild(Entity parent, Entity child)
{
    if (getParent(child).valid())
    {
        DFLOG_WARN("Can not add child entity. An entity already has a parent");
        return;
    }

    auto &parentCompData = m_pimpl->data.getData(parent);
    auto &childCompData = m_pimpl->data.getData(child);

    parentCompData.children.push_back(child);
    childCompData.parent = parent;

    m_pimpl->updateWorldTransformation(parentCompData);
}

void SceneGraphComponentProcessor::attachChildren(Entity parent, const std::vector<Entity> &children)
{
    for (auto c : children)
    {
        DF3D_ASSERT_MESS(!getParent(c).valid(), "already have a parent");
        m_pimpl->data.getData(c).parent = parent;
    }

    auto &parentCompData = m_pimpl->data.getData(parent);
    parentCompData.children.insert(parentCompData.children.end(), children.begin(), children.end());

    m_pimpl->updateWorldTransformation(parentCompData);
}

void SceneGraphComponentProcessor::detachChild(Entity parent, Entity child)
{
    if (!getParent(child).valid())
    {
        DFLOG_WARN("Can not detach entity. An entity has no parent");
        return;
    }

    auto &parentData = m_pimpl->data.getData(parent);
    auto &childData = m_pimpl->data.getData(child);

    childData.parent = Entity();

    auto found = std::find(parentData.children.begin(), parentData.children.end(), child);
    DF3D_ASSERT(found != parentData.children.end());

    parentData.children.erase(found);
    m_pimpl->updateWorldTransformation(childData);
}

void SceneGraphComponentProcessor::detachAllChildren(Entity e)
{
    auto &compData = m_pimpl->data.getData(e);

    for (auto childEnt : compData.children)
    {
        m_pimpl->data.getData(childEnt).parent = Entity();
        m_pimpl->updateWorldTransformation(m_pimpl->data.getData(childEnt));
    }

    compData.children.clear();
}

Entity SceneGraphComponentProcessor::getParent(Entity e)
{
    return m_pimpl->data.getData(e).parent;
}

const std::vector<Entity>& SceneGraphComponentProcessor::getChildren(Entity e) const
{
    return m_pimpl->data.getData(e).children;
}

void SceneGraphComponentProcessor::add(Entity e)
{
    if (m_pimpl->data.contains(e))
    {
        DFLOG_WARN("An entity already has a scene graph component");
        return;
    }

    Impl::Data data;
    data.holder = e;

    m_pimpl->data.add(e, data);

    m_pimpl->updateWorldTransformation(m_pimpl->data.getData(e));
}

}
