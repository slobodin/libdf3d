#include "EntityManager.h"

namespace df3d { namespace game_impl {

Entity::IdType EntityManager::getNextId()
{
    if (!m_available.empty())
    {
        auto res = m_available.front();
        m_available.pop_front();
        return res;
    }

    return m_next++;
}

EntityManager::EntityManager()
{

}

EntityManager::~EntityManager()
{

}

Entity EntityManager::create()
{
    Entity e;
    e.id = getNextId();

    m_entities.insert(e);

    return e;
}

void EntityManager::destroy(Entity e)
{
    assert(e.valid());

    auto count = m_entities.erase(e);
    assert(count == 1);
    // NOTE: component data should be destroyed later via World::cleanStep

    m_removed.push_back(e.id);
}

bool EntityManager::alive(Entity e) const
{
    assert(e.valid());

    return m_entities.find(e) != m_entities.end();
}

size_t EntityManager::size() const
{
    return m_entities.size();
}

void EntityManager::cleanStep()
{
    m_available.insert(m_available.end(), m_removed.begin(), m_removed.end());
    m_removed.clear();
}

} }
