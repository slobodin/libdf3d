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
    Entity e = m_bag.getNew();

    m_entities.insert(e);

    return e;
}

void EntityManager::destroy(Entity e)
{
    DF3D_ASSERT_MESS(e.valid(), "can't destroy invalid entity");

    DF3D_VERIFY(m_entities.erase(e) == 1);
    // NOTE: component data should be destroyed later via World::cleanStep

    m_bag.release(e);
}

bool EntityManager::alive(Entity e) const
{
    DF3D_ASSERT_MESS(e.valid(), "invalid entity");

    return m_entities.find(e) != m_entities.end();
}

size_t EntityManager::size() const
{
    return m_entities.size();
}

void EntityManager::cleanStep()
{
    m_bag.cleanup();
}

} }
