#pragma once

#include <scene/Entity.h>

namespace df3d {

// TODO_ecs : add test suits.
class DF3D_DLL EntityManager : utils::NonCopyable
{
public:
    EntityManager();
    ~EntityManager();

    Entity create();
    void destroy(Entity e);
};

}
