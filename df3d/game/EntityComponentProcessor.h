#pragma once

#include "Entity.h"

namespace df3d {

class RenderQueue;

class DF3D_DLL EntityComponentProcessor : NonCopyable
{
public:
    EntityComponentProcessor() = default;
    virtual ~EntityComponentProcessor() = default;

    virtual void update() = 0;
    virtual void draw(RenderQueue *ops) { }
    virtual bool has(Entity e) = 0;
    virtual void remove(Entity e) = 0;
};

}
