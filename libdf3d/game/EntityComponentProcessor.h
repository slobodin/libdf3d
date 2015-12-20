#pragma once

namespace df3d {

class RenderQueue;
class World;

class DF3D_DLL EntityComponentProcessor : utils::NonCopyable
{
public:
    EntityComponentProcessor() = default;
    virtual ~EntityComponentProcessor() = default;

    virtual void update(float systemDelta, float gameDelta) = 0;
    virtual void cleanStep(World &w) = 0;
};

}

