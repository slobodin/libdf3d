#pragma once

#include <libdf3d/game/Entity.h>

namespace df3d { namespace game_impl {

class DF3D_DLL EntityManager : utils::NonCopyable
{
    std::unordered_set<Entity> m_entities;
    Entity::IdType m_next = 0;

    std::list<Entity::IdType> m_removed;
    std::list<Entity::IdType> m_available;
    Entity::IdType getNextId();

public:
    EntityManager();
    ~EntityManager();

    Entity create();
    void destroy(Entity e);
    bool alive(Entity e) const;
    size_t size() const;

    void cleanStep();
};

} }
