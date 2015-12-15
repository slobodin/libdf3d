#include "df3d_pch.h"
#include "World.h"

#include "impl/EntityManager.h"
#include <audio/AudioComponentProcessor.h>
#include <render/StaticMeshComponentProcessor.h>

namespace df3d {

World::World()
    : m_entityManager(new EntityManager()),
    m_audio(new AudioComponentProcessor()),
    m_staticMeshes(new StaticMeshComponentProcessor())
{

}

World::World(const std::string &worldResource)
{

}

World::~World()
{

}

Entity World::spawn()
{
    return m_entityManager->create();
}

Entity World::spawn(const std::string &entityResource)
{
    return m_entityManager->create();
}

bool World::alive(Entity e)
{
    return m_entityManager->alive(e);
}

}
