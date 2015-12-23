#pragma once

#include "Entity.h"

namespace df3d {

class DF3D_DLL EntityComponentProcessor : utils::NonCopyable
{
public:
    EntityComponentProcessor() = default;
    virtual ~EntityComponentProcessor() = default;

    virtual void update() = 0;
    virtual void cleanStep(const std::list<Entity> &deleted) = 0;
};

}

