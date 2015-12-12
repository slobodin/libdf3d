#pragma once

#include "Entity.h"

namespace df3d {

class EntityManager;

class DF3D_DLL World : utils::NonCopyable
{
    unique_ptr<EntityManager> m_entityManager;

public:
    World();
    World(const std::string &worldResource);
    ~World();

    Entity spawn();
    Entity spawn(const std::string &entityResource);
    //Entity spawn(const std::string &entityResource /* position */);
    //Entity spawn(const std::string &entityResource /* transformation */);

    // TODO: get processors.
};

}
