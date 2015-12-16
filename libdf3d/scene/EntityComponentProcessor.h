#pragma once

namespace df3d {

class RenderQueue;
class World;

class DF3D_DLL EntityComponentProcessor : utils::NonCopyable
{
    friend class World;

protected:
    virtual void draw(RenderQueue *ops) { }
    virtual void update(float systemDelta, float gameDelta) { }
    virtual void cleanStep(World &w) { }

public:
    EntityComponentProcessor() = default;
    virtual ~EntityComponentProcessor() = default;
};

}

