#include "EntityManager.h"

namespace df3d { namespace game_impl {

EntityManager::EntityManager()
    : m_bag(std::numeric_limits<int32_t>::max() - 1)
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

// FIXME:
// Need this step at the end of the frame because data is being deleted 
// from the processors also at the end of the frame.
// So we can delete an entity with id 1 and then create new with the same id
// but processors won't know this before ::cleanstep
void EntityManager::cleanStep()
{
    m_bag.cleanup();
}

} }
