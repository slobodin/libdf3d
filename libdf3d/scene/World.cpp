#include "df3d_pch.h"
#include "World.h"

#include "impl/EntityManager.h"
#include <audio/AudioComponentProcessor.h>

namespace df3d {

World::World()
    : m_entityManager(new EntityManager()),
    m_audio(new AudioComponentProcessor())
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

}
