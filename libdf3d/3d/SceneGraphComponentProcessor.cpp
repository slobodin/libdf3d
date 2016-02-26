#include "SceneGraphComponentProcessor.h"

#include <libdf3d/game/ComponentDataHolder.h>
#include <libdf3d/utils/MathUtils.h>

namespace df3d {

struct SceneGraphComponentProcessor::Impl
{
    // TODO_ecs: play with layout.
    // TODO_ecs: return dirty flag back?
    struct Data
    {
        //! Accumulated node world transform (including parent).
        glm::mat4 worldTransform;

        glm::mat4 localTransform;
        bool localTransformDirty = true;
        //! Node local position.
        glm::vec3 position;
        //! Node local orientation.
        glm::quat orientation;
        //! Node local scale.
        glm::vec3 scaling = glm::vec3(1.0f, 1.0f, 1.0f);

        std::string name;

        Entity holder;
        Entity parent;
        std::vector<Entity> children;
    };

    ComponentDataHolder<Data> data;

    void updateWorldTransformation(Data &component)
    {
        updateLocalTransform(component);

        if (component.parent.valid())
            component.worldTransform = data.getData(component.parent).worldTransform * component.localTransform; // tr = parent * me
        else
            component.worldTransform = component.localTransform;

        updateChildren(component);
    }

    void updateWorldTransformation(Data &component, const glm::mat4 &parentTransform)
    {
        updateLocalTransform(component);

        component.worldTransform = parentTransform * component.localTransform; // tr = parent * me

        for (auto child : component.children)
            updateWorldTransformation(data.getData(child), component.worldTransform);
    }

    void updateLocalTransform(Data &component)
    {
        if (component.localTransformDirty)
        {
            // Scale -> Rotation -> Translation
            component.localTransform = glm::translate(component.position) * glm::toMat4(component.orientation) * glm::scale(component.scaling);
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
    //glog << "SceneGraphComponentProcessor::~SceneGraphComponentProcessor alive entities" << m_pimpl->data.rawData().size() << logdebug;
}

void SceneGraphComponentProcessor::setPosition(Entity e, const glm::vec3 &newPosition)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.position = newPosition;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setScale(Entity e, const glm::vec3 &newScale)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.scaling = newScale;
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

    compData.orientation = newOrientation;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setOrientation(Entity e, const glm::vec3 &eulerAngles)
{
    setOrientation(e, glm::quat(glm::radians(eulerAngles)));
}

void SceneGraphComponentProcessor::setTransform(Entity e, const glm::vec3 &position, const glm::quat &orient, const glm::mat4 &transf)
{
    auto &compData = m_pimpl->data.getData(e);
    compData.position = position;
    compData.orientation = orient;
    compData.localTransform = transf * glm::scale(compData.scaling);
    compData.localTransformDirty = false;

    glm::mat4 parentTransf = compData.parent.valid() ? m_pimpl->data.getData(compData.parent).worldTransform : glm::mat4();
    m_pimpl->updateWorldTransformation(compData, parentTransf);
}

void SceneGraphComponentProcessor::translate(Entity e, const glm::vec3 &v)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.position += v;
    compData.localTransformDirty = true;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::scale(Entity e, const glm::vec3 &v)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.scaling *= v;
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
    compData.orientation = q * compData.orientation;
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

    assert(parent.valid());

    for (const auto &compData : m_pimpl->data.rawData())
    {
        if (compData.parent == parent && compData.name == name)
            return compData.holder;
    }

    return {};
}

glm::vec3 SceneGraphComponentProcessor::getWorldPosition(Entity e) const
{
    return glm::vec3(m_pimpl->data.getData(e).worldTransform[3]);
}

glm::vec3 SceneGraphComponentProcessor::getLocalPosition(Entity e) const
{
    return m_pimpl->data.getData(e).position;
}

const glm::vec3& SceneGraphComponentProcessor::getLocalScale(Entity e) const
{
    return m_pimpl->data.getData(e).scaling;
}

const glm::quat& SceneGraphComponentProcessor::getOrientation(Entity e) const
{
    return m_pimpl->data.getData(e).orientation;
}

const glm::mat4& SceneGraphComponentProcessor::getWorldTransform(Entity e) const
{
    return m_pimpl->data.getData(e).worldTransform;
}

void SceneGraphComponentProcessor::getWorldTransformMeshWorkaround(Entity e, glm::mat4 &outTr, glm::vec3 &outPos, glm::quat &outRot, glm::vec3 &outScale) const
{
    const auto &compData = m_pimpl->data.getData(e);

    outTr = compData.worldTransform;
    outPos = glm::vec3(compData.worldTransform[3]);
    outRot = compData.orientation;
    outScale = compData.scaling;
}

glm::vec3 SceneGraphComponentProcessor::getRotation(Entity e) const
{
    const auto &compData = m_pimpl->data.getData(e);

    return glm::degrees(glm::eulerAngles(compData.orientation));
}

glm::vec3 SceneGraphComponentProcessor::getWorldDirection(Entity e) const
{
    return glm::normalize(glm::vec3(getWorldTransform(e) * -utils::math::ZAxis));
}

glm::vec3 SceneGraphComponentProcessor::getWorldUp(Entity e) const
{
    return glm::normalize(glm::vec3(getWorldTransform(e) * utils::math::YAxis));
}

glm::vec3 SceneGraphComponentProcessor::getWorldRight(Entity e) const
{
    return glm::normalize(glm::vec3(getWorldTransform(e) * utils::math::XAxis));
}

void SceneGraphComponentProcessor::attachChild(Entity parent, Entity child)
{
    if (getParent(child).valid())
    {
        glog << "Can not add child entity. An entity already has a parent" << logwarn;
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
        assert(!getParent(c).valid());
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
        glog << "Can not detach entity. An entity has no parent" << logwarn;
        return;
    }

    auto &parentData = m_pimpl->data.getData(parent);
    auto &childData = m_pimpl->data.getData(child);

    childData.parent = Entity();

    auto found = std::find(parentData.children.begin(), parentData.children.end(), child);
    assert(found != parentData.children.end());

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
        glog << "An entity already has a scene graph component" << logwarn;
        return;
    }

    Impl::Data data;
    data.holder = e;

    m_pimpl->data.add(e, data);
}

}
