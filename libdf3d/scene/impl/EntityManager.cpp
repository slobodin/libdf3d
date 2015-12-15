#include "df3d_pch.h"
#include "EntityManager.h"

namespace df3d {

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
}

bool EntityManager::alive(Entity e)
{
    assert(e.valid());

    return m_entities.find(e.id) != m_entities.end();
}

}
