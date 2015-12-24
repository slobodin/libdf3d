#include "EntityManager.h"

namespace df3d { namespace game_impl {

EntityManager::EntityManager()
{

}

EntityManager::~EntityManager()
{

}

Entity EntityManager::create()
{
    m_next.id++;

    m_entities.insert(m_next.id);

    return m_next;
}

void EntityManager::destroy(Entity e)
{
    assert(e.valid());

    m_entities.erase(e.id);
    // NOTE: component data should be destroyed later via World::cleanStep
}

bool EntityManager::alive(Entity e) const
{
    assert(e.valid());

    return m_entities.find(e.id) != m_entities.end();
}

size_t EntityManager::size() const
{
    return m_entities.size();
}

} }
