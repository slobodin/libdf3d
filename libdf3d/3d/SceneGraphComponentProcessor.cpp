#include "SceneGraphComponentProcessor.h"

#include <game/ComponentDataHolder.h>

namespace df3d {

struct SceneGraphComponentProcessor::Impl
{
    // TODO_ecs: play with layout.
    // TODO_ecs: return dirty flag back?
    struct Data
    {
        //! Accumulated node world transform (including parent).
        glm::mat4 transformation;
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
        // Scale -> Rotation -> Translation
        auto tr = glm::translate(component.position) * glm::toMat4(component.orientation) * glm::scale(component.scaling);

        if (component.parent.valid())
            component.transformation = data.getData(component.parent).transformation * tr; // tr = parent * me
        else
            component.transformation = tr;

        updateChildren(component);
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

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setPosition(Entity e, float x, float y, float z)
{
    setPosition(e, glm::vec3(x, y, z));
}

void SceneGraphComponentProcessor::setScale(Entity e, const glm::vec3 &newScale)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.scaling = newScale;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setScale(Entity e, float x, float y, float z)
{
    setScale(e, glm::vec3(x, y, z));
}

void SceneGraphComponentProcessor::setScale(Entity e, float uniform)
{
    setScale(e, uniform, uniform, uniform);
}

void SceneGraphComponentProcessor::setOrientation(Entity e, const glm::quat &newOrientation)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.orientation = newOrientation;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::setOrientation(Entity e, const glm::vec3 &eulerAngles, bool rads)
{
    glm::vec3 v = rads ? eulerAngles : glm::radians(eulerAngles);

    setOrientation(e, glm::quat(v));
}

void SceneGraphComponentProcessor::setTransform(Entity e, const glm::vec3 &position, const glm::quat &orient, const glm::mat4 &transf)
{
    auto &compData = m_pimpl->data.getData(e);
    compData.position = position;
    compData.orientation = orient;
    compData.transformation = transf * glm::scale(compData.scaling);

    m_pimpl->updateChildren(compData);
}

void SceneGraphComponentProcessor::translate(Entity e, const glm::vec3 &v)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.position += v;

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::translate(Entity e, float x, float y, float z)
{
    translate(e, glm::vec3(x, y, z));
}

void SceneGraphComponentProcessor::scale(Entity e, const glm::vec3 &v)
{
    scale(e, v.x, v.y, v.z);
}

void SceneGraphComponentProcessor::scale(Entity e, float x, float y, float z)
{
    auto &compData = m_pimpl->data.getData(e);

    compData.scaling *= glm::vec3(x, y, z);

    m_pimpl->updateWorldTransformation(compData);
}

void SceneGraphComponentProcessor::scale(Entity e, float uniform)
{
    scale(e, uniform, uniform, uniform);
}

void SceneGraphComponentProcessor::rotateYaw(Entity e, float yaw, bool rads)
{
    rotateAxis(e, yaw, glm::vec3(0.0f, 1.0f, 0.0f), rads);
}

void SceneGraphComponentProcessor::rotatePitch(Entity e, float pitch, bool rads)
{
    rotateAxis(e, pitch, glm::vec3(1.0f, 0.0f, 0.0f), rads);
}

void SceneGraphComponentProcessor::rotateRoll(Entity e, float roll, bool rads)
{
    rotateAxis(e, roll, glm::vec3(0.0f, 0.0f, 1.0f), rads);
}

void SceneGraphComponentProcessor::rotateAxis(Entity e, float angle, const glm::vec3 &axis, bool rads)
{
    if (!rads)
        angle = glm::radians(angle);

    auto q = glm::angleAxis(angle, axis);
    q = glm::normalize(q);

    auto &compData = m_pimpl->data.getData(e);
    compData.orientation = q * compData.orientation;

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
    for (const auto &compData : m_pimpl->data.rawData())
    {
        if (!compData.parent.valid() && compData.name == name)
            return compData.holder;
    }

    return Entity();
}

Entity SceneGraphComponentProcessor::getByName(Entity parent, const std::string &name) const
{
    assert(parent.valid());

    for (const auto &compData : m_pimpl->data.rawData())
    {
        if (compData.parent == parent && compData.name == name)
            return compData.holder;
    }

    return Entity();
}

glm::vec3 SceneGraphComponentProcessor::getPosition(Entity e, bool includeParent)
{
    const auto &compData = m_pimpl->data.getData(e);

    if (!includeParent)
        return compData.position;
    else
        return glm::vec3(compData.transformation[3]);
}

const glm::vec3& SceneGraphComponentProcessor::getScale(Entity e)
{
    return m_pimpl->data.getData(e).scaling;
}

const glm::quat& SceneGraphComponentProcessor::getOrientation(Entity e)
{
    return m_pimpl->data.getData(e).orientation;
}

const glm::mat4& SceneGraphComponentProcessor::getTransformation(Entity e)
{
    return m_pimpl->data.getData(e).transformation;
}

glm::vec3 SceneGraphComponentProcessor::getRotation(Entity e, bool rads)
{
    const auto &compData = m_pimpl->data.getData(e);

    if (rads)
        return glm::eulerAngles(compData.orientation);
    else
        return glm::degrees(glm::eulerAngles(compData.orientation));
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
