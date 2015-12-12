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
    return Entity(32);
}

void EntityManager::destroy(Entity e)
{

}

}
