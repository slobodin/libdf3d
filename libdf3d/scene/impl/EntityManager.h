#pragma once

#include <scene/Entity.h>

namespace df3d {

class DF3D_DLL EntityManager : utils::NonCopyable
{
public:
    EntityManager();
    ~EntityManager();

    Entity create();
    void destroy(Entity e);
};

}
