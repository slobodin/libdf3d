#pragma once

#include <game/Entity.h>

namespace df3d { namespace scene_impl {

class EntityManager : utils::NonCopyable
{
    std::unordered_set<Entity::IdType> m_entities;
    Entity m_next;

public:
    EntityManager();
    ~EntityManager();

    Entity create();
    void destroy(Entity e);
    bool alive(Entity e) const;
    size_t size() const;
};

} }
